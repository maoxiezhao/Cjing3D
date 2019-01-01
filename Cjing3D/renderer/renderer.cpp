#include "renderer.h"
#include "shaderLib.h"
#include "stateManager.h"
#include "renderer\RHI\device.h"
#include "core\systemContext.hpp"
#include "core\eventSystem.h"
#include "helper\profiler.h"
#include "renderer\RHI\deviceD3D11.h"
#include "world\component\camera.h"
#include "world\component\renderable.h"
#include "world\world.h"
#include "resource\resourceManager.h"
#include "renderer\components\material.h"

namespace Cjing3D {

namespace {
	GraphicsDevice* CreateGraphicsDeviceByType(RenderingDeviceType deviceType, HWND window)
	{
		GraphicsDevice* graphicsDevice = nullptr;
		switch (deviceType)
		{
		case RenderingDeviceType_D3D11:
			graphicsDevice = new GraphicsDeviceD3D11(window, false, true);
			break;
		}
		return graphicsDevice;
	}

	const size_t MAX_FRAME_ALLOCATOR_SIZE = 4 * 1024 * 1024;
}

Renderer::Renderer(SystemContext& gameContext, RenderingDeviceType deviceType, HWND window) :
	SubSystem(gameContext),
	mGraphicsDevice(nullptr),
	mShaderLib(nullptr),
	mStateManager(nullptr),
	mIsInitialized(false),
	mIsRendering(false),
	mCamera(nullptr),
	mFrameNum(0)
{
	mGraphicsDevice = std::unique_ptr<GraphicsDevice>(CreateGraphicsDeviceByType(deviceType, window));
}

Renderer::~Renderer()
{
}

void Renderer::Initialize()
{
	if (mIsInitialized == true){
		return;
	}

	U32x2 screenSize = mGraphicsDevice->GetScreenSize();

	// initialize device
	mGraphicsDevice->Initialize();

	// initialize camera
	mCamera = std::make_shared<Camera>(mGameContext);
	mCamera->SetupPerspective(screenSize[0], screenSize[1], 0.1f, 1000.0f);

	// initialize states
	mStateManager = std::make_unique<StateManager>(*mGraphicsDevice);
	mStateManager->SetupStates();

	// initialize shader
	mShaderLib = std::make_unique<ShaderLib>(*this);
	mShaderLib->Initialize();

	// initialize mip generator
	mDeferredMIPGenerator = std::make_unique<DeferredMIPGenerator>(*mGraphicsDevice);

	// initialize render queue;
	mRenderQueue = std::make_unique<RenderQueue>();

	// initialize frame allocator
	mFrameAllocator = std::make_unique<LinearAllocator>();
	mFrameAllocator->Reserve(MAX_FRAME_ALLOCATOR_SIZE);

	InitializePasses();

	mIsInitialized = true;
}

void Renderer::Update()
{
	UpdateRenderData();
}

void Renderer::Uninitialize()
{
	if (mIsInitialized == false) {
		return;
	}

	mGraphicsDevice->Uninitialize();

	mIsInitialized = false;
}

void Renderer::Render()
{
	ForwardRender();

	if (mCamera == nullptr) {
		mIsRendering = false;
		return;
	}

	return;

	// global data
	{
		mNearPlane = mCamera->GetNearPlane();
		mFrameNum = mCamera->GetFarPlane();
		mView = mCamera->GetViewMatrix();
		mProjection = mCamera->GetProjectionMatrix();
		mViewProjection = mCamera->GetViewProjectionMatrix();
	}

	mIsRendering = true;
	mFrameNum++;

	//PassGBuffer();

	mIsRendering = false;
}

void Renderer::Present()
{
	mGraphicsDevice->PresentBegin();
	mGraphicsDevice->PresentEnd();
}

GraphicsDevice & Renderer::GetDevice()
{
	return *mGraphicsDevice;
}

ResourceManager & Renderer::GetResourceManager()
{
	return mGameContext.GetSubSystem<ResourceManager>();
}

Pipeline & Renderer::GetPipeline()
{
	return *mPipeline;
}

DeferredMIPGenerator & Renderer::GetDeferredMIPGenerator()
{
	return *mDeferredMIPGenerator;
}

CameraPtr Renderer::GetCamera()
{
	return mCamera;
}

void Renderer::RenderSceneOpaque(std::shared_ptr<Camera> camera, RenderingType renderingType)
{
	FrameCullings& frameCulling = mFrameCullings[camera];
	auto& renderingActors = frameCulling.mRenderingActors;

	mRenderQueue->Clear();
	for (auto& actor : renderingActors)
	{
		auto renderables = actor->GetComponents<Renderable>();
		for (auto& renderable : renderables)
		{
			if (renderable->GetRenderingType() & RenderableType_Opaque)
			{
				// 创建一个RenderBatch,其中包含必要的渲染所需信息
				// TODO; 分布动态分配带来的消耗，将使用内存池缓解
				RenderBatch* batch = (RenderBatch*)mFrameAllocator->Allocate(sizeof(RenderBatch));
				if (batch == nullptr)
				{
					Debug::Error("Not enough memory to allocator RenderBatch");
					break;
				}

				batch->Init(*renderable);	// ???
				mRenderQueue->AddBatch(batch);
			}
		}
	}

	if (mRenderQueue->IsEmpty() == false)
	{
		// ??? TODO
		XMMATRIX viewProj = camera->GetViewProjectionMatrix();
		size_t batchCount = mRenderQueue->GetBatchCount();

		mRenderQueue->Sort();
		ProcessRenderQueue(*mRenderQueue, renderingType, viewProj);
		mRenderQueue->Clear();

		mFrameAllocator->Free(batchCount * sizeof(RenderBatch));
	}
}

void Renderer::RenderSceneTransparent(std::shared_ptr<Camera> camera, RenderingType renderingType)
{
}

void Renderer::UpdateCameraCB(Camera & camera)
{
}

ShaderInfoState Renderer::GetShaderInfoState(Material & material)
{
	return mShaderLib->GetShaderInfoState(material);
}

void Renderer::InitializePasses()
{
	mForwardPass = std::make_unique<ForwardPass>(mGameContext, *this);
}

/**
*	\brief 接受参与渲染的actor， 在world渲染时传入actors
*/
void Renderer::AccquireActors(std::vector<ActorPtr> actors)
{
	for (auto& kvp : mFrameCullings) {
		auto& frameCulling = kvp.second;
		frameCulling.Clear();

		CameraPtr camera = kvp.first;
		if (camera == nullptr) {
			continue;
		}
		auto currentFrustum = camera->GetFrustum();
		frameCulling.mFrustum = currentFrustum;

		for (const auto& actor : actors)
		{
			if (actor == nullptr) {
				continue;
			}

			// Cull renderable
			auto renderables = actor->GetComponents<Renderable>();
			for (auto& renderable : renderables)
			{
				if (renderable != nullptr && currentFrustum.Overlaps(renderable->GetBoundingBox()) == true)
				{
					frameCulling.mRenderingActors.push_back(actor);
				}
			}
		}
	}
}

// 更新渲染数据
void Renderer::UpdateRenderData()
{
	// 处理延时生成的mipmap
	if (mDeferredMIPGenerator != nullptr) {
		mDeferredMIPGenerator->UpdateMipGenerating();
	}
}

void Renderer::ProcessRenderQueue(RenderQueue & queue, RenderingType renderingType, XMMATRIX viewProj)
{
	if (queue.IsEmpty() == false)
	{
		auto& resourceManager = GetGameContext().GetSubSystem<ResourceManager>();
		auto& world = GetGameContext().GetSubSystem<World>();

		U32 prevMeshGUID = 0;
		U32 instanceSize = 0;
		U32 instanceCount = 0;

		std::vector<RenderBatchInstance*> instances;
		U32 bathIndex = 0;
		auto& renderBatches = queue.GetRenderBatches();
		for (auto& batch : renderBatches)
		{
			U32 meshGUID = batch->GetMeshGUID();
			if (meshGUID != prevMeshGUID)
			{
				prevMeshGUID = meshGUID;
				instanceCount++;

				RenderBatchInstance* instance = (RenderBatchInstance*)mFrameAllocator->Allocate(sizeof(RenderBatchInstance));
				instance->mMesh = batch->GetMesh();
				instance->mDataOffset = bathIndex * instanceSize;

				instances.push_back(instance);
			}

			Actor* actor = nullptr; //batch->GetActor();
			if (actor == nullptr) {
				continue;
			}

			XMMATRIX worldMatrix = actor->GetTransform().GetMatrix() * viewProj;
			bathIndex++;
		}

		for (auto& instance : instances)
		{
			MeshPtr mesh = instance->mMesh;

			// bind index buffer
			mGraphicsDevice->BindIndexBuffer(mesh->GetIndexBuffer(), mesh->GetIndexFormat(), 0);

			bool bindVertexBuffer = false;
			for (auto& subset : mesh->GetMeshSubsets())
			{
				MaterialPtr material = world.GetMaterialByGUID(subset.mMaterialID); 
				if (material == nullptr) {
					continue;
				}
				
				ShaderInfoState state = mShaderLib->GetShaderInfoState(*material);
				if (state.IsEmpty()) {
					continue;
				}

				// bind vertex buffer
				if (bindVertexBuffer == false)
				{
					GPUBuffer* vbs[] = {
						&mesh->GetVertexBuffer()
					};
					U32 strides[] = {
						sizeof(Mesh::VertexPosNormalTex)
					};
					U32 offsets[] = {
						0
					};
					mGraphicsDevice->BindVertexBuffer(vbs, 0, 1, strides, offsets);
				}

				// bind shader state
				mGraphicsDevice->BindShaderInfoState(&state);

				// bind material texture
				GPUResource* resources[] = {
					material->GetBaseColorMap().get(),
					material->GetSurfaceMap().get(),
					material->GetNormalMap().get(),
				};
				mGraphicsDevice->BindGPUResources(SHADERSTAGES_PS, resources, TEXTURE_SLOT_0, 3);
			
				// draw
				mGraphicsDevice->DrawIndexedInstances(subset.mIndexCount, instance->mInstanceCount, instance->mDataOffset, 0, 0);
			}
		}

		mFrameAllocator->Free(instanceCount * sizeof(RenderBatchInstance));
	}
}

void Renderer::ForwardRender()
{
	mForwardPass->Render();
}

// 以延迟渲染方式绘制不透明物体
void Renderer::PassGBuffer()
{
	//if (mRenderingActors[RenderableType::RenderableType_Opaque].empty()) {
	//	return;
	//}

	auto& profiler = Profiler::GetInstance();
	profiler.BeginBlock("GBuffer_Pass");

	auto gBufferShader = mShaderLib->GetVertexShader(VertexShaderType_Transform);

	mPipeline->SetPrimitiveTopology(PRIMITIVE_TOPOLOGY::TRIANGLELIST);
	mPipeline->SetFillMode(FILL_SOLID);
	//mPipeline->SetVertexShader(gBufferShader);

	profiler.EndBlock();
}

void Renderer::FrameCullings::Clear()
{
	mRenderingActors.clear();
}

}