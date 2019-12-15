#include "renderer.h"
#include "shaderLib.h"
#include "stateManager.h"
#include "pipelineStateInfoManager.h"
#include "bufferManager.h"
#include "renderer2D.h"
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

void RenderFrameData::Clear()
{
	mShaderLightArrayCount = 0;
}

Renderer::Renderer(SystemContext& gameContext, RenderingDeviceType deviceType, HWND window) :
	SubSystem(gameContext),
	mGraphicsDevice(nullptr),
	mShaderLib(nullptr),
	mStateManager(nullptr),
	mPipelineStateInfoManager(nullptr),
	mIsInitialized(false),
	mIsRendering(false),
	mCamera(nullptr),
	mCurrentRenderPath(nullptr),
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

	// initialize resource manager
	mBufferManager = std::make_unique<BufferManager>(*this);
	mBufferManager->Initialize();

	// initialize pipelineStateInfos
	mPipelineStateInfoManager = std::make_unique<PipelineStateInfoManager>(*this);
	mPipelineStateInfoManager->SetupPipelineStateInfos();

	// initialize mip generator
	mDeferredMIPGenerator = std::make_unique<DeferredMIPGenerator>(*mGraphicsDevice);

	// initialize frame allocator
	mFrameAllocator = std::make_unique<LinearAllocator>();
	mFrameAllocator->Reserve(MAX_FRAME_ALLOCATOR_SIZE);

	// initialize 2d renderer
	mRenderer2D = std::make_unique<Renderer2D>(*this);
	mRenderer2D->Initialize();

	// initialize frame cullings
	mFrameCullings[GetCamera()] = FrameCullings();

	mFrameData.mFrameScreenSize = { (F32)mScreenSize[0], (F32)mScreenSize[1] };
	mFrameData.mFrameAmbient = { 0.1f, 0.1f, 0.1f };

	mIsInitialized = true;
}

void Renderer::Uninitialize()
{
	if (mIsInitialized == false) {
		return;
	}

	GetMainScene().Clear();

	if (mCurrentRenderPath != nullptr)
	{
		mCurrentRenderPath->Uninitialize();
		mCurrentRenderPath.reset();
	}

	mRenderer2D->Uninitialize();
	mRenderer2D.reset();
	mFrameAllocator.reset();
	mPipelineStateInfoManager.reset();
	mBufferManager->Uninitialize();
	mBufferManager.reset();
	mShaderLib->Uninitialize();
	mShaderLib.reset();
	mStateManager.reset();

	mGraphicsDevice->Uninitialize();

	mIsInitialized = false;
}

void Renderer::Update(F32 deltaTime)
{
	if (mCurrentRenderPath != nullptr) {
		mCurrentRenderPath->Update(deltaTime);
	}
}

// 在update中执行
void Renderer::UpdatFrameData(F32 deltaTime)
{
	// update scene system
	auto& mainScene = GetMainScene();
	mainScene.Update(deltaTime);

	CameraPtr mainCamera = GetCamera();
	mainCamera->Update();

	// update materials
	mPendingUpdateMaterials.clear();

	auto& materials = mainScene.GetComponentManager<MaterialComponent>();
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

	// culling and get available entity
	auto& scene = GetMainScene();
	for (auto& kvp : mFrameCullings) {
		auto& frameCulling = kvp.second;
		frameCulling.Clear();

		// 只处理当前主相机的culling
		CameraPtr camera = kvp.first;
		if (camera == nullptr && camera != mainCamera) {
			continue;
		}

		auto currentFrustum = camera->GetFrustum();
		frameCulling.mFrustum = currentFrustum;

		// 遍历场景所有物体的aabb,如果在相机范围内，则添加物体的index
		auto& objectAABBs = scene.mObjectAABBs;
		for (size_t i = 0; i < objectAABBs.GetCount(); i++)
		{
			auto aabb = objectAABBs[i];
			if (aabb != nullptr && currentFrustum.Overlaps(*aabb) == true)
			{
				frameCulling.mCulledObjects.push_back((U32)i);
			}
		}

		// 遍历场景的light aabbs
		auto& lightAABBs = scene.mLightAABBs;
		for (size_t i = 0; i < lightAABBs.GetCount(); i++)
		{
			auto aabb = lightAABBs[i];
			if (aabb != nullptr && currentFrustum.Overlaps(*aabb) == true)
			{
				frameCulling.mCulledLights.push_back((U32)i);
			}
		}
	}
}

// 在render阶段render前执行
void Renderer::UpdateRenderData()
{
	mFrameData.Clear();

	auto& mainScene = GetMainScene();

	// 处理延时生成的mipmap
	if (mDeferredMIPGenerator != nullptr) {
		mDeferredMIPGenerator->UpdateMipGenerating();
	}

	CameraPtr mainCamera = GetCamera();
	if (mainCamera == nullptr) {
		return;
	}

	// 处理更新material data 
	for (int materialIndex : mPendingUpdateMaterials)
	{
		auto material = mainScene.mMaterials[materialIndex];

		ShaderMaterial sm = material->CreateMaterialCB();
		mGraphicsDevice->UpdateBuffer(material->GetConstantBuffer(), &sm, sizeof(sm));
	}

	// 更新shaderLightArray buffer
	ShaderLight* shaderLights = (ShaderLight*)mFrameAllocator->Allocate(sizeof(ShaderLight) * SHADER_LIGHT_COUNT);

	XMMATRIX viewMatrix = mainCamera->GetViewMatrix();
	FrameCullings& frameCulling = mFrameCullings.at(mainCamera);
	U32 lightCount = 0;
	auto& lights = frameCulling.mCulledLights;
	for (const auto& lightIndex : lights)
	{
		if (lightCount >= SHADER_LIGHT_COUNT) {
			Debug::Warning("Invalid light count" + std::to_string(lightCount) + " " + std::to_string(SHADER_LIGHT_COUNT));
			break;
		}

		std::shared_ptr<LightComponent> light = mainScene.GetComponentByIndex<LightComponent>(lightIndex);
		if (light == nullptr) {
			continue;
		}

		ShaderLight shaderLight = light->CreateShaderLight(viewMatrix);
		shaderLights[lightCount] = shaderLight;

		lightCount++;
	}
	mFrameData.mShaderLightArrayCount = lightCount;

	GPUBuffer& lightsBuffer = mBufferManager->GetResourceBuffer(ResourceBufferType_ShaderLight);
	mGraphicsDevice->UpdateBuffer(lightsBuffer, shaderLights, sizeof(ShaderLight) * lightCount);

	mFrameAllocator->Free(sizeof(ShaderLight) * SHADER_LIGHT_COUNT);

	// 更新每一帧的基本信息
	UpdateFrameCB();
}

void Renderer::Render()
{
	mIsRendering = true;

	mRenderer2D->Render();

	if (mCurrentRenderPath != nullptr) {
		mCurrentRenderPath->Render();
	}

	mIsRendering = false;
}

void Renderer::Compose()
{
	if (mCurrentRenderPath != nullptr) {
		mCurrentRenderPath->Compose();
	}
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
	mGraphicsDevice->BeginEvent("RenderSceneOpaque");

	// bind constant buffer
	BindConstanceBuffer(SHADERSTAGES_VS);
	BindConstanceBuffer(SHADERSTAGES_PS);

	auto& scene = GetMainScene();

	RenderQueue renderQueue;
	FrameCullings& frameCulling = mFrameCullings[camera];
	auto& objects = frameCulling.mCulledObjects;
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

	mGraphicsDevice->EndEvent();
}

void Renderer::RenderSceneTransparent(std::shared_ptr<CameraComponent> camera, ShaderType renderingType)
{
	mGraphicsDevice->BeginEvent("RenderSceneTransparent");


	mGraphicsDevice->EndEvent();
}

void Renderer::RenderPostprocess(Texture2D & input, Texture2D & output)
{
	mGraphicsDevice->BeginEvent("Postprocess");


	mGraphicsDevice->EndEvent();
}

void Renderer::BindCommonResource()
{
	SamplerState& linearClampGreater = *mStateManager->GetSamplerState(SamplerStateID_LinearClampGreater);

	for (int stageIndex = 0; stageIndex < SHADERSTAGES_COUNT; stageIndex++)
	{
		SHADERSTAGES stage = static_cast<SHADERSTAGES>(stageIndex);
		mGraphicsDevice->BindSamplerState(stage, linearClampGreater, SAMPLER_LINEAR_CLAMP_SLOT);
	}

	// bind shader light resource
	GPUResource* resources[] = {
		&mBufferManager->GetResourceBuffer(ResourceBufferType_ShaderLight)
	};
	mGraphicsDevice->BindGPUResources(SHADERSTAGES_PS, resources, SHADER_LIGHT_ARRAY_SLOT, 1);
}

void Renderer::UpdateCameraCB(CameraComponent & camera)
{
	CameraCB cb;

	DirectX::XMStoreFloat4x4(&cb.gCameraVP,   camera.GetViewProjectionMatrix());
	DirectX::XMStoreFloat4x4(&cb.gCameraView, camera.GetViewMatrix());
	DirectX::XMStoreFloat4x4(&cb.gCameraProj, camera.GetProjectionMatrix());

	cb.gCameraPos = XMConvert(camera.GetCameraPos());

	GPUBuffer& cameraBuffer = mBufferManager->GetConstantBuffer(ConstantBufferType_Camera);
	GetDevice().UpdateBuffer(cameraBuffer, &cb, sizeof(CameraCB));
}

void Renderer::UpdateFrameCB()
{
	FrameCB frameCB;
	frameCB.gFrameScreenSize = XMConvert(mFrameData.mFrameScreenSize);
	frameCB.gFrameAmbient = XMConvert(mFrameData.mFrameAmbient);
	frameCB.gShaderLightArrayCount = mFrameData.mShaderLightArrayCount;

	GPUBuffer& frameBuffer = mBufferManager->GetConstantBuffer(ConstantBufferType_Frame);
	mGraphicsDevice->UpdateBuffer(frameBuffer, &frameCB, sizeof(FrameCB));
}

Scene & Renderer::GetMainScene()
{
	return Scene::GetScene();
}

PipelineStateInfoManager & Renderer::GetPipelineStateInfoManager()
{
	return *mPipelineStateInfoManager;
}

Renderer2D & Renderer::GetRenderer2D()
{
	return *mRenderer2D;
}

void Renderer::SetCurrentRenderPath(RenderPath * renderPath)
{
	if (mCurrentRenderPath != nullptr)
	{
		mCurrentRenderPath->Stop();
		mCurrentRenderPath->Uninitialize();
	}

	mCurrentRenderPath = std::unique_ptr<RenderPath>(renderPath);

	if (mCurrentRenderPath != nullptr)
	{
		mCurrentRenderPath->Initialize();
		mCurrentRenderPath->Start();
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
		// 用于在shader中计算顶点的世界坐标
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

			PipelineStateInfo state = mPipelineStateInfoManager->GetPipelineStateInfo(shaderType, *material);  // TODO
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
	GPUBuffer& cameraBuffer = mBufferManager->GetConstantBuffer(ConstantBufferType_Camera);
	mGraphicsDevice->BindConstantBuffer(stage, cameraBuffer, CB_GETSLOT_NAME(CameraCB));

	GPUBuffer& frameBuffer = mBufferManager->GetConstantBuffer(ConstantBufferType_Frame);
	mGraphicsDevice->BindConstantBuffer(stage, frameBuffer, CB_GETSLOT_NAME(FrameCB));
}

void Renderer::FrameCullings::Clear()
{
	mCulledObjects.clear();
	mCulledLights.clear();
}


}