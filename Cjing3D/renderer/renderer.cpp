#include "renderer.h"
#include "shaderLib.h"
#include "stateManager.h"
#include "renderer\RHI\device.h"
#include "core\systemContext.hpp"
#include "core\eventSystem.h"
#include "helper\profiler.h"
#include "renderer\RHI\deviceD3D11.h"
#include "system\component\camera.h"
#include "resource\resourceManager.h"
#include "system\sceneSystem.h"

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
	mCamera->SetupPerspective((F32)mScreenSize[0], (F32)mScreenSize[1], 0.1f, 1000.0f);

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

	InitializeRenderPaths();

	mIsInitialized = true;
}

void Renderer::Uninitialize()
{
	if (mIsInitialized == false) {
		return;
	}

	mRenderPathForward->Uninitialize();

	mGraphicsDevice->Uninitialize();

	mIsInitialized = false;
}

void Renderer::Update(F32 deltaTime)
{
	mPendingUpdateMaterials.clear();

	auto& scene = GetMainScene();
	auto& materials = scene.GetComponentManager<MaterialComponent>();
	for (int index = 0; index < materials.GetCount(); index++)
	{
		auto material = materials[index];
		if (material->IsDirty())
		{
			material->SetIsDirty(false);
			material->SetupConstantBuffer(*mGraphicsDevice);

			mPendingUpdateMaterials.push_back(index);
		}
	}
}

// do it before rendering
void Renderer::SetupRenderFrame()
{
	UpdateRenderData();
}

void Renderer::Render()
{
	mIsRendering = true;

	ForwardRender();

	mIsRendering = false;
}

void Renderer::Compose()
{
	mRenderPathForward->Compose();
}

void Renderer::Present()
{
	mGraphicsDevice->PresentBegin();
	Compose();
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
	// bind constant buffer
	BindConstanceBuffer(SHADERSTAGES_VS);
	BindConstanceBuffer(SHADERSTAGES_PS);

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

Scene & Renderer::GetMainScene()
{
	return Scene::GetScene();
}

void Renderer::InitializeRenderPaths()
{
	mRenderPathForward = std::make_unique<RenderPathForward>(*this);
	mRenderPathForward->Initialize();
}

// 更新渲染数据
void Renderer::UpdateRenderData()
{
	// 处理延时生成的mipmap
	if (mDeferredMIPGenerator != nullptr) {
		mDeferredMIPGenerator->UpdateMipGenerating();
	}

	auto& mainScene = GetMainScene();
	// 处理更新material data 
	for (int materialIndex : mPendingUpdateMaterials)
	{
		auto material = mainScene.mMaterials[materialIndex];

		MaterialCB cb;
		cb.gMaterialBaseColor = XMConvert(material->mBaseColor);

		mGraphicsDevice->UpdateBuffer(material->GetConstantBuffer(), &cb, sizeof(cb));
	}

	// update render scene
	auto& scene = GetMainScene();
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

void Renderer::ProcessRenderQueue(RenderQueue & queue, ShaderType shaderType, RenderableType renderableType)
{
	if (queue.IsEmpty() == true) {
		return;
	}

	auto& scene = GetMainScene();

	size_t instanceSize = sizeof(RenderInstance);
	GraphicsDevice::GPUAllocation instances = mGraphicsDevice->AllocateGPU(queue.GetBatchCount() * instanceSize);

	// 对于使用相同mesh的object将创建同一个instanceBatch，包含相同的shader，
	// 但是依然会为每个object创建一个instance（input layout）包含了颜色和世界坐标变换矩阵

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

			// bind material constant buffer
			mGraphicsDevice->BindConstantBuffer(SHADERSTAGES_VS, material->GetConstantBuffer(), CB_GETSLOT_NAME(MaterialCB));
			mGraphicsDevice->BindConstantBuffer(SHADERSTAGES_PS, material->GetConstantBuffer(), CB_GETSLOT_NAME(MaterialCB));

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

void Renderer::BindConstanceBuffer(SHADERSTAGES stage)
{
	auto& cameraBuffer = *mShaderLib->GetConstantBuffer(ConstantBufferType_Camera);

	mGraphicsDevice->BindConstantBuffer(stage, cameraBuffer, CB_GETSLOT_NAME(CameraCB));
}

void Renderer::ForwardRender()
{
	mRenderPathForward->Render();
}

void Renderer::FrameCullings::Clear()
{
	mRenderingObjects.clear();
}

}