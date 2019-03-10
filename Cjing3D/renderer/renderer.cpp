#include "renderer.h"
#include "shaderLib.h"
#include "stateManager.h"
#include "renderer\RHI\device.h"
#include "core\systemContext.hpp"
#include "core\eventSystem.h"
#include "helper\profiler.h"
#include "renderer\RHI\deviceD3D11.h"
#include "world\component\camera.h"
#include "world\world.h"
#include "resource\resourceManager.h"

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

	// initialize device
	mGraphicsDevice->Initialize();

	mScreenSize = mGraphicsDevice->GetScreenSize();

	// initialize camera
	mCamera = std::make_shared<CameraComponent>();
	mCamera->SetupPerspective(mScreenSize[0], mScreenSize[1], 0.1f, 1000.0f);

	// initialize states
	mStateManager = std::make_unique<StateManager>(*mGraphicsDevice);
	mStateManager->SetupStates();

	// initialize shader
	mShaderLib = std::make_unique<ShaderLib>(*this);
	mShaderLib->Initialize();

	// initialize mip generator
	mDeferredMIPGenerator = std::make_unique<DeferredMIPGenerator>(*mGraphicsDevice);

	// initialize frame allocator
	mFrameAllocator = std::make_unique<LinearAllocator>();
	mFrameAllocator->Reserve(MAX_FRAME_ALLOCATOR_SIZE);

	InitializePasses();

	mIsInitialized = true;
}

void Renderer::Update()
{
	UpdateRenderData();
	UpdateRenderingScene();
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
	mIsRendering = true;

	ForwardRender();

	mIsRendering = false;
}

void Renderer::Compose()
{
	mForwardPass->Compose();
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

ShaderLib & Renderer::GetShaderLib()
{
	return *mShaderLib;
}

StateManager & Renderer::GetStateManager()
{
	return *mStateManager;
}

void Renderer::RenderSceneOpaque(std::shared_ptr<CameraComponent> camera, ShaderType shaderType)
{
	auto& scene = GetMainScene();

	RenderQueue renderQueue;

	FrameCullings& frameCulling = mFrameCullings[camera];
	auto& objects = frameCulling.mRenderingObjects;
	for (const auto& objectIndex : objects)
	{
		auto objectEntity = scene.GetEntityByIndex<ObjectComponent>(objectIndex);
		auto object = scene.GetComponent<ObjectComponent>(objectEntity);

		if (object == nullptr  ||
			object->GetObjectType() != ObjectComponent::OjbectType_Renderable ||
			object->GetRenderableType() != RenderableType_Opaque)
		{
			continue;
		}

		RenderBatch* renderBatch = (RenderBatch*)mFrameAllocator->Allocate(sizeof(RenderBatch));
		renderBatch->Init(objectEntity, object->mMeshID);

		renderQueue.AddBatch(renderBatch);
	}

	if (renderQueue.IsEmpty() == false)
	{
		renderQueue.Sort();
		ProcessRenderQueue(renderQueue, shaderType, RenderableType_Opaque);

		mFrameAllocator->Free(renderQueue.GetBatchCount() * sizeof(RenderBatch));
	}
}

void Renderer::RenderSceneTransparent(std::shared_ptr<CameraComponent> camera, ShaderType renderingType)
{
}

void Renderer::UpdateCameraCB(CameraComponent & camera)
{
	CameraCB cb;

	// 在shader中是右乘
	DirectX::XMStoreFloat4x4(&cb.gCameraVP,   XMMatrixTranspose(camera.GetViewProjectionMatrix()));
	DirectX::XMStoreFloat4x4(&cb.gCameraView, XMMatrixTranspose(camera.GetViewProjectionMatrix()));
	DirectX::XMStoreFloat4x4(&cb.gCameraProj, XMMatrixTranspose(camera.GetViewProjectionMatrix()));
	cb.gCameraPos = XMConvert(camera.GetCameraPos());

	auto cameraBuffer = mShaderLib->GetConstantBuffer(ConstantBufferType_Camera);
	GetDevice().UpdateBuffer(*cameraBuffer, &cb, sizeof(CameraCB));
}

ShaderInfoState Renderer::GetShaderInfoState(MaterialComponent & material)
{
	return ShaderInfoState();
}

Scene & Renderer::GetMainScene()
{
	auto& world = GetGameContext().GetSubSystem<World>();
	return world.GetMainScene();
}

void Renderer::InitializePasses()
{
	mForwardPass = std::make_unique<ForwardPass>(mGameContext, *this);
}

// 更新渲染数据
void Renderer::UpdateRenderData()
{
	// 处理延时生成的mipmap
	if (mDeferredMIPGenerator != nullptr) {
		mDeferredMIPGenerator->UpdateMipGenerating();
	}
}

void Renderer::ProcessRenderQueue(RenderQueue & queue, ShaderType shaderType, RenderableType renderableType)
{
	if (queue.IsEmpty() == true) {
		return;
	}

	auto& scene = GetMainScene();

	size_t instanceSize = sizeof(RenderInstance);
	GraphicsDevice::GPUAllocation instances = mGraphicsDevice->AllocateGPU(queue.GetBatchCount() * instanceSize);

	// 将共同的mesh创建为同一个instancedBatch
	ECS::Entity prevMeshEntity = ECS::INVALID_ENTITY;
	U32 instancedBatchCount = 0;

	std::vector<RenderBatchInstance*> mRenderBatchInstances;

	auto& renderBatches = queue.GetRenderBatches();
	size_t batchesCount = renderBatches.size();
	for (size_t index = 0; index < batchesCount; index++)
	{
		auto& batch = renderBatches[index];
		auto meshEntity = batch->GetMeshEntity();
		auto objectEntity = batch->GetObjectEntity();

		if (meshEntity != prevMeshEntity)
		{
			prevMeshEntity = meshEntity;
			instancedBatchCount++;

			RenderBatchInstance* batchInstance = (RenderBatchInstance*)mFrameAllocator->Allocate(sizeof(RenderBatchInstance));
			batchInstance->mMeshEntity = meshEntity;
			batchInstance->mDataOffset = instances.offset + index * instanceSize;
			batchInstance->mInstanceCount = 0;

			mRenderBatchInstances.push_back(batchInstance);
		}

		const auto object = scene.GetComponent<ObjectComponent>(objectEntity);
		const auto transform = scene.GetComponent<TransformComponent>(objectEntity);

		// 保存每个batch（包含一个object）的worldMatrix
		const XMFLOAT4X4 worldMatrix = transform->GetWorldTransform();
		const XMFLOAT4 color = XMConvert(object->mColor) ;
		((RenderInstance*)instances.data)[index].Setup(worldMatrix, color);

		// 当前batchInstance必然在array最后
		mRenderBatchInstances.back()->mInstanceCount++;
	}

	// process render batch instances
	for (auto& bathInstance : mRenderBatchInstances)
	{
		const auto mesh = scene.GetComponent<MeshComponent>(bathInstance->mMeshEntity);

		mGraphicsDevice->BindIndexBuffer(mesh->GetIndexBuffer(), mesh->GetIndexFormat(), 0);

		bool bindVertexBuffer = false;
		for (auto& subset : mesh->GetSubsets())
		{
			auto material = scene.GetComponent<MaterialComponent>(subset.mMaterialID);
			if (material == nullptr) {
				continue;
			}

			ShaderInfoState state = mShaderLib->GetShaderInfoState(shaderType, *material);  // TODO
			if (state.IsEmpty()) {
				continue;
			}

			bool is_renderable = false;
			if (renderableType == RenderableType_Opaque)
			{
				is_renderable = true;
			}

			if (is_renderable == false)
			{
				continue;
			}
		
			// bind vertex buffer
			if (bindVertexBuffer == false)
			{
				bindVertexBuffer = true;

				GPUBuffer* vbs[] = {
					&mesh->GetVertexBufferPos(),
					&mesh->GetVertexBufferTex(),
					instances.buffer
				};
				U32 strides[] = {
					sizeof(MeshComponent::VertexPosNormalSubset),
					sizeof(MeshComponent::VertexTex),
					instanceSize
				};
				U32 offsets[] = {
					0,
					0,
					bathInstance->mDataOffset	// 因为所有的batch公用一个buffer，所以提供offset
				};
				mGraphicsDevice->BindVertexBuffer(vbs, 0, 3, strides, offsets);
			}

			// bind material state
			mGraphicsDevice->BindShaderInfoState(state);

			// bind material texture
			GPUResource* resources[] = {
				material->mBaseColorMap.get(),
				material->mSurfaceMap.get(),
				material->mNormalMap.get(),
			};
			mGraphicsDevice->BindGPUResources(SHADERSTAGES_PS, resources, TEXTURE_SLOT_0, 3);

			// draw
			mGraphicsDevice->DrawIndexedInstances(subset.mIndexCount, bathInstance->mInstanceCount, subset.mIndexOffset, 0, 0);
		}
	}

	mFrameAllocator->Free(instancedBatchCount * sizeof(RenderBatchInstance));
}

// 更新场景，并更新FrameCullings
void Renderer::UpdateRenderingScene()
{
	auto& world = GetGameContext().GetSubSystem<World>();
	auto& scene = world.GetMainScene();

	for (auto& kvp : mFrameCullings) {
		auto& frameCulling = kvp.second;
		frameCulling.Clear();

		CameraPtr camera = kvp.first;
		if (camera == nullptr) {
			continue;
		}

		camera->Update();
		auto currentFrustum = camera->GetFrustum();
		frameCulling.mFrustum = currentFrustum;

		// 遍历场景所有物体的aabb,如果在相机范围内，则添加物体的index
		auto& objectAABBs = scene.mObjectAABBs;
		for (size_t i = 0; i < objectAABBs.GetCount(); i++)
		{
			auto aabb = objectAABBs[i];
			if (aabb != nullptr && currentFrustum.Overlaps(*aabb) == true)
			{
				frameCulling.mRenderingObjects.push_back((U32)i);
			}
		}


	}
}

void Renderer::ForwardRender()
{
	mForwardPass->Render();
}

void Renderer::FrameCullings::Clear()
{
	mRenderingObjects.clear();
}

}