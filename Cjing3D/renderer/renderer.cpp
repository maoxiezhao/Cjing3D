#include "renderer.h"
#include "shaderLib.h"
#include "stateManager.h"
#include "pipelineStateManager.h"
#include "bufferManager.h"
#include "renderer2D.h"
#include "textureHelper.h"
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

///////////////////////////////////////////////////////////////////////////////////
//  renderer variants
///////////////////////////////////////////////////////////////////////////////////
	// definitions, only use in this cpp.
	const U32 MAX_FRAME_ALLOCATOR_SIZE = 4 * 1024 * 1024;

///////////////////////////////////////////////////////////////////////////////////
//  shadow pass
///////////////////////////////////////////////////////////////////////////////////
	// ��ǰ��Ӱ��������
	U32 shadowCascadeCount = 3;
	F32 shadowCascadeCoefficient = 0.75;
	U32 shadowRes2DResolution = 1024;
	U32 shadowMap2DCount = 8;
	std::vector<F32> currentCascadeSplits;

	Texture2D shadowMapArray2D;

	// ��������׶��
	struct CascadeShadowCamera
	{
		AABB mBoundingBox;
		XMFLOAT4X4 mView;
		XMFLOAT4X4 mProjection;

		XMMATRIX GetViewProjectionMatrix()
		{
			return XMLoadFloat4x4(&mView) * XMLoadFloat4x4(&mProjection);
		}
	};

	void SetShadowMap2DProperty(Renderer& renderer, U32 resolution, U32 count)
	{
		// ��Ӱ��ͼ��һϵ�н����������Ϣ����ͼ
		shadowRes2DResolution = resolution;
		shadowMap2DCount = count;

		if (resolution <= 0 || count <= 0) {
			return;
		}

		TextureDesc desc = {};
		desc.mWidth = resolution;
		desc.mHeight = resolution;
		desc.mArraySize = count;
		desc.mMipLevels = 1;
		desc.mFormat = FORMAT_R16_TYPELESS;
		desc.mBindFlags = BIND_DEPTH_STENCIL | BIND_SHADER_RESOURCE;
		
		GraphicsDevice& device = renderer.GetDevice();
		const auto result = device.CreateTexture2D(&desc, nullptr, shadowMapArray2D);
		Debug::ThrowIfFailed(result, "Failed to create shadow map array 2d:%08x", result);
		device.SetResourceName(shadowMapArray2D, "ShadowMapArray2D");

		for (U32 i = 0; i < count; i++) {
			device.CreateDepthStencilView(shadowMapArray2D, i, 1);
		}
	}

	void CalculateCascadeSplits(CameraComponent& camera, U32 cascadeCount, std::vector<F32>& cascadeSplits)
	{
		// ������׶�弶���ָ�
		// reference:Parallel-split shadow maps for large-scale virtual environments;
		// z = r * n(f/n)^(i/n) + (1 - r)(n + (i/n)(f-n))
		cascadeSplits.clear();

		if (cascadeCount <= 0) {
			return;
		}

		F32 nearPlane = camera.GetNearPlane();
		F32 farPlane = camera.GetFarPlane();
		F32 fDivN = farPlane / nearPlane;
		F32 invFSubN = 1.0f / (farPlane - nearPlane);

		for (U32 cascade = 0; cascade <= cascadeCount; cascade++)
		{
			F32 cascadeDivCount = F32(cascade) / F32(cascadeCount);
			F32 logSplit = nearPlane * std::powf(fDivN, cascadeDivCount);
			F32 uniformSplit = nearPlane + cascadeDivCount * (farPlane - nearPlane);

			F32 split = shadowCascadeCoefficient * logSplit + (1 - shadowCascadeCoefficient) * uniformSplit;
			split = (split - nearPlane) * invFSubN;

			cascadeSplits.push_back(split);
		}
	}

	void CreateCascadeShadowCameras(LightComponent& light, CameraComponent& camera, U32 cascadeCount, std::vector<CascadeShadowCamera>& csfs)
	{
		if (currentCascadeSplits.size() != (cascadeCount + 1)) {
			CalculateCascadeSplits(camera, cascadeCount, currentCascadeSplits);
		}

		F32 nearPlane = camera.GetNearPlane();
		F32 farPlane = camera.GetFarPlane();

		// convert main frustum into world space, 1 is near, 0 is far(reverse zbuffer)
		const XMMATRIX invVP = camera.GetInvViewProjectionMatrix();
		const XMVECTOR frustumCorners[] =
		{
			XMVector3TransformCoord(XMVectorSet(-1, -1, 1, 1), invVP),	// near
			XMVector3TransformCoord(XMVectorSet(-1, 1, 1, 1),  invVP),	// near
			XMVector3TransformCoord(XMVectorSet(1, -1, 1, 1),  invVP),	// near
			XMVector3TransformCoord(XMVectorSet(1, 1, 1, 1),   invVP),	// near

			XMVector3TransformCoord(XMVectorSet(-1, -1, 0, 1), invVP),	// far
			XMVector3TransformCoord(XMVectorSet(-1, 1, 0, 1),  invVP),	// far
			XMVector3TransformCoord(XMVectorSet(1, -1, 0, 1),  invVP),	// far
			XMVector3TransformCoord(XMVectorSet(1, 1, 0, 1),   invVP),	// far
		};

		// create light camera view matrix
		// directional light default dir is (0, -1, 0)
		const XMMATRIX rotation = XMMatrixRotationQuaternion(XMLoad(light.mRotation));
		XMVECTOR up = XMVector3TransformNormal(XMVectorSet(0, 0,  1, 0), rotation);
		XMVECTOR to = XMVector3TransformNormal(XMVectorSet(0, -1, 0, 0), rotation);
		XMMATRIX lightView = XMMatrixLookToLH(XMVectorZero(), to, up);
		XMMATRIX invLightView = XMMatrixInverse(nullptr, lightView);

		// ��ÿ���㼶������Ӧ����Ӱ���
		// ����������׶��ת������������ռ�󣬴���boundingBox��view projection
		for (U32 cascade = 0; cascade < cascadeCount; cascade++)
		{
			const F32 split_near = currentCascadeSplits[cascade];
			const F32 split_far  = currentCascadeSplits[cascade + 1];

			// ������׶��ת����light space
			const XMVECTOR corners[] =
			{
				XMVector3Transform(XMVectorLerp(frustumCorners[0], frustumCorners[4], split_near), lightView),
				XMVector3Transform(XMVectorLerp(frustumCorners[1], frustumCorners[5], split_near), lightView),
				XMVector3Transform(XMVectorLerp(frustumCorners[2], frustumCorners[6], split_near), lightView),
				XMVector3Transform(XMVectorLerp(frustumCorners[3], frustumCorners[7], split_near), lightView),

				XMVector3Transform(XMVectorLerp(frustumCorners[0], frustumCorners[4], split_far), lightView),
				XMVector3Transform(XMVectorLerp(frustumCorners[1], frustumCorners[5], split_far), lightView),
				XMVector3Transform(XMVectorLerp(frustumCorners[2], frustumCorners[6], split_far), lightView),
				XMVector3Transform(XMVectorLerp(frustumCorners[3], frustumCorners[7], split_far), lightView),
			};

			// compute cascade bounding box
			XMVECTOR center = XMVectorZero();
			for (auto corner : corners) {
				center += corner;
			}
			center = center / F32(ARRAYSIZE(corners));

			F32 radius = 0.0f;
			for (auto corner : corners) {
				radius = std::max(radius, XMVectorGetX(XMVector3Length(XMVectorSubtract(corner, center))));
			}

			XMVECTOR vMin = center + XMVectorReplicate(-radius);
			XMVECTOR vMax = center + XMVectorReplicate(radius);

			// ���뵽����������
			const XMVECTOR texelSize = XMVectorSubtract(vMax, vMin) / (F32)shadowRes2DResolution;
			vMin = XMVectorFloor(vMin / texelSize) * texelSize;
			vMax = XMVectorFloor(vMax / texelSize) * texelSize;
			
			// ����׶�屾���Χ����z�᷽���С�������Χ���԰�����������壨������׶��֮������壬���ڹ�Դ������Ҳ���ܲ�����Ӱ��
			XMFLOAT3 _min, _max;
			XMStoreFloat3(&_min, vMin);
			XMStoreFloat3(&_max, vMax);
			_min.z = std::min(_min.z, -farPlane * 0.5f);
			_max.z = std::max(_max.z,  farPlane * 0.5f);

			auto& cam = csfs.emplace_back();

			// ��Χ����Ҫ��ת������������
			AABB boundingBox(XMLoadFloat3(&_min), XMLoadFloat3(&_max));
			cam.mBoundingBox = boundingBox.GetByTransforming(invLightView);

			// light view and projection matrix, reversed z
			const XMMATRIX lightProjection = XMMatrixOrthographicOffCenterLH(_min.x, _max.x, _min.y, _max.y, _max.z, _min.z); 
			XMStoreFloat4x4(&cam.mView, lightView);
			XMStoreFloat4x4(&cam.mProjection, lightProjection);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////

void RenderFrameData::Clear()
{
	mShaderLightArrayCount = 0;
}

Renderer::Renderer(SystemContext& gameContext, RenderingDeviceType deviceType, HWND window) :
	SubSystem(gameContext),
	mGraphicsDevice(nullptr),
	mShaderLib(nullptr),
	mStateManager(nullptr),
	mPipelineStateManager(nullptr),
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
	mStateManager->Initialize();

	// initialize shader
	mShaderLib = std::make_unique<ShaderLib>(*this);
	mShaderLib->Initialize();

	// initialize resource manager
	mBufferManager = std::make_unique<BufferManager>(*this);
	mBufferManager->Initialize();

	// initialize mip generator
	mDeferredMIPGenerator = std::make_unique<DeferredMIPGenerator>(*this);

	// initialize 2d renderer
	mRenderer2D = std::make_unique<Renderer2D>(*this);
	mRenderer2D->Initialize();

	// initialize texture helper
	TextureHelper::Initialize();

	// initialize PipelineStates
	mPipelineStateManager = std::make_unique<PipelineStateManager>(*this);
	mPipelineStateManager->Initialize();

	// initialize frame cullings
	mFrameCullings[GetCamera()] = FrameCullings();

	mFrameData.mFrameScreenSize = { (F32)mScreenSize[0], (F32)mScreenSize[1] };
	mFrameData.mFrameAmbient = { 0.1f, 0.1f, 0.1f };


	SetShadowMap2DProperty(*this, shadowRes2DResolution, shadowMap2DCount);

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

	TextureHelper::Uninitialize();

	mRenderer2D->Uninitialize();
	mRenderer2D.reset();

	mPipelineStateManager->Uninitialize();
	mPipelineStateManager.reset();

	mBufferManager->Uninitialize();
	mBufferManager.reset();

	mShaderLib->Uninitialize();
	mShaderLib.reset();

	mStateManager->Uninitalize();
	mStateManager.reset();

	mGraphicsDevice->Uninitialize();
	mGraphicsDevice = nullptr;

	mIsInitialized = false;
}

void Renderer::Update(F32 deltaTime)
{
	if (mCurrentRenderPath != nullptr) {
		mCurrentRenderPath->Update(deltaTime);
	}
}

// ��update��ִ��
void Renderer::UpdatePerFrameData(F32 deltaTime)
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

		// ֻ����ǰ�������culling
		CameraPtr camera = kvp.first;
		if (camera == nullptr && camera != mainCamera) {
			continue;
		}

		auto currentFrustum = camera->GetFrustum();
		frameCulling.mFrustum = currentFrustum;

		// �����������������aabb,����������Χ�ڣ�����������index
		auto& objectAABBs = scene.mObjectAABBs;
		for (size_t i = 0; i < objectAABBs.GetCount(); i++)
		{
			auto aabb = objectAABBs[i];
			if (aabb != nullptr && currentFrustum.Overlaps(aabb->GetAABB()) == true)
			{
				frameCulling.mCulledObjects.push_back((U32)i);
			}
		}

		// ����������light aabbs
		auto& lightAABBs = scene.mLightAABBs;
		for (size_t i = 0; i < lightAABBs.GetCount(); i++)
		{
			auto aabb = lightAABBs[i];
			if (aabb != nullptr && currentFrustum.Overlaps(aabb->GetAABB()) == true)
			{
				frameCulling.mCulledLights.push_back((U32)i);
			}
		}

		LinearAllocator& frameAllocator = GetFrameAllocator(FrameAllocatorType_Render);

		F32x3 cameraPos = camera->GetCameraPos();
		U32 culledLightCount = frameCulling.mCulledLights.size();
		if (culledLightCount > 0)
		{
			auto& culledLights = frameCulling.mCulledLights;

			// ���ڲü����Ĺ�Դ�����ݹ�Դ�ɽ���Զ����shadowMap
			// ����һ���������飬ÿ������ͬʱ�����Դ�ľ����������ǰ��16λ����һ�������������
			U32* lightSortingHashes = (U32*)frameAllocator.Allocate(culledLightCount * sizeof(U32));
			U32 hashIndex = 0;
			for(U32 i = 0; i < culledLightCount; i++)
			{ 
				auto light = scene.mLights[culledLights[i]];
				if (light == nullptr) {
					continue;
				}
				
				F32 distance = DistanceEstimated(cameraPos, light->mPosition);
				lightSortingHashes[hashIndex] = i & 0xffff;
				lightSortingHashes[hashIndex] |= (U32(distance * 10) & 0xffff) << 16;

				hashIndex++;
			}
			std::sort(lightSortingHashes, lightSortingHashes + hashIndex, std::less<U32>());

			U32 currentShadowMap2DCount = 0;
			for (U32 i = 0; i < hashIndex; i++)
			{
				U32 lightIndex = lightSortingHashes[culledLights[i]] & 0xffff;
				auto light = scene.mLights[lightIndex];
				if (light == nullptr) {
					continue;
				}

				// Ĭ������Ӱ��ͼ
				light->SetShadowMapIndex(-1);

				if (light->IsCastShadow() == false) {
					continue;
				}

				switch (light->GetLightType())
				{
				case Cjing3D::LightComponent::LightType_Directional:
					if ((currentShadowMap2DCount + shadowCascadeCount) <= shadowMap2DCount)
					{
						light->SetShadowMapIndex(currentShadowMap2DCount);
						currentShadowMap2DCount += shadowCascadeCount;
					}
					break;
				default:
					break;
				}
			}

			frameAllocator.Free(culledLightCount * sizeof(U32));
		}
	}
}

// ��render�׶�renderǰִ��
void Renderer::RefreshRenderData()
{
	mFrameData.Clear();

	auto& mainScene = GetMainScene();

	// ������ʱ���ɵ�mipmap
	if (mDeferredMIPGenerator != nullptr) {
		mDeferredMIPGenerator->UpdateMipGenerating();
	}

	CameraPtr mainCamera = GetCamera();
	if (mainCamera == nullptr) {
		return;
	}

	// �������material data 
	for (int materialIndex : mPendingUpdateMaterials)
	{
		auto material = mainScene.mMaterials[materialIndex];

		ShaderMaterial sm = material->CreateMaterialCB();
		mGraphicsDevice->UpdateBuffer(material->GetConstantBuffer(), &sm, sizeof(sm));
	}

	LinearAllocator& frameAllocator = GetFrameAllocator(FrameAllocatorType_Render);

	// ����shaderLightArray buffer
	ShaderLight* shaderLights = (ShaderLight*)frameAllocator.Allocate(sizeof(ShaderLight) * SHADER_LIGHT_COUNT);
	U32 lightCount = 0;

	// ����shaderMatrixArray buffer
	XMMATRIX* shaderMatrixs = (XMMATRIX*)frameAllocator.Allocate(sizeof(XMMATRIX) * SHADER_MATRIX_COUNT);
	U32 currentMatrixIndex = 0;

	XMMATRIX viewMatrix = mainCamera->GetViewMatrix();
	FrameCullings& frameCulling = mFrameCullings.at(mainCamera);
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

		// ���light֧����Ӱ�����ѷ�����shadowMapIndex���򴫵ݶ�Ӧ
		// shader viewProjection Matrix��shaderMapIndex
		if (light->IsCastShadow() && light->GetShadowMapIndex() >= 0)
		{
			shaderLight.SetShadowData(currentMatrixIndex, light->GetShadowMapIndex());
			switch (light->GetLightType())
			{
			case Cjing3D::LightComponent::LightType_Directional:
			{
				// TODO:��renderShadow�����ּ�����һ�Ρ���
				std::vector<CascadeShadowCamera> cascadeShadowCameras;
				CreateCascadeShadowCameras(*light, *GetCamera(), shadowCascadeCount, cascadeShadowCameras);

				for (auto& cam : cascadeShadowCameras) {
					shaderMatrixs[currentMatrixIndex++] = cam.GetViewProjectionMatrix();
				}
			}
			break;

			default:
				break;
			}
		}

		shaderLights[lightCount] = shaderLight;
		lightCount++;
	}
	mFrameData.mShaderLightArrayCount = lightCount;

	// update buffer
	mGraphicsDevice->UpdateBuffer(mBufferManager->GetStructuredBuffer(StructuredBufferType_ShaderLight), 
		shaderLights, sizeof(ShaderLight) * lightCount);
	mGraphicsDevice->UpdateBuffer(mBufferManager->GetStructuredBuffer(StructuredBufferType_MatrixArray),
		shaderMatrixs, sizeof(XMMATRIX) * currentMatrixIndex);

	frameAllocator.Free(sizeof(XMMATRIX) * SHADER_MATRIX_COUNT);
	frameAllocator.Free(sizeof(ShaderLight) * SHADER_LIGHT_COUNT);

	// ����ÿһ֡�Ļ�����Ϣ
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

U32x2 Renderer::GetScreenSize()
{
	return mScreenSize;
}

GraphicsDevice & Renderer::GetDevice()
{
	return *mGraphicsDevice;
}

ResourceManager & Renderer::GetResourceManager()
{
	return mGameContext.GetSubSystem<ResourceManager>();
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

BufferManager& Renderer::GetBufferManager()
{
	return *mBufferManager;
}

U32 Renderer::GetShadowCascadeCount() const
{
	return shadowCascadeCount;
}

U32 Renderer::GetShadowRes2DResolution() const
{
	return shadowRes2DResolution;
}

F32x3 Renderer::GetAmbientColor() const
{
	return mFrameData.mFrameAmbient;
}

void Renderer::SetAmbientColor(F32x3 color)
{
	mFrameData.mFrameAmbient = color;
}

void Renderer::RenderShadowmaps(std::shared_ptr<CameraComponent> camera)
{
	FrameCullings& frameCulling = mFrameCullings[camera];
	auto& culledLights = frameCulling.mCulledLights;
	if (culledLights.empty()) {
		return;
	}
	auto& scene = GetMainScene();

	mGraphicsDevice->BeginEvent("RenderShadowmaps");

	// Ϊ��д��shadowmap,��Ҫ��Unbind GPUResource
	mGraphicsDevice->UnbindGPUResources(TEXTURE_SLOT_SHADOW_ARRAY_2D, 1);

	// ���ݹ�Դ�������δ���cascade shadowmaps
	for (U32 lightTypeIndex = 0; lightTypeIndex < LightComponent::LightType_Count; lightTypeIndex++)
	{
		LightComponent::LightType lightType = static_cast<LightComponent::LightType>(lightTypeIndex);
		
		for (auto lightIndex : culledLights)
		{
			auto light = scene.mLights[lightIndex];

			if ( light == nullptr || 
				!light->IsCastShadow() || 
				 light->GetLightType() != lightType) {
				continue;
			}

			switch (lightType)
			{
			case Cjing3D::LightComponent::LightType_Directional:
				RenderDirLightShadowmap(*light, *camera);
				break;
			case Cjing3D::LightComponent::LightType_Point:
				break;
			default:
				break;
			}
		}
	}

	mGraphicsDevice->BindRenderTarget(0, nullptr, nullptr);
	mGraphicsDevice->EndEvent();
}

void Renderer::RenderSceneOpaque(std::shared_ptr<CameraComponent> camera, RenderPassType renderPassType)
{
	mGraphicsDevice->BeginEvent("RenderSceneOpaque");

	BindShadowMaps(SHADERSTAGES_PS);

	if (camera != nullptr) {
		RenderImpostor(*camera, renderPassType);
	}

	LinearAllocator& frameAllocator = GetFrameAllocator(FrameAllocatorType_Render);
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
			object->GetRenderableType() != RenderableType_Opaque) {
			continue;
		}

		// Ĭ�ϵ�object����ΪImpostor��������RenderImpostorʱ��Ⱦ
		// δ�����ǻ�����һ��distance������distanceʱ�Ž�����ȾImpostor
		if (object->IsImpostor()) {
			continue;;
		}

		RenderBatch* renderBatch = (RenderBatch*)frameAllocator.Allocate(sizeof(RenderBatch));
		renderBatch->Init(objectEntity, object->mMeshID);

		renderQueue.AddBatch(renderBatch);
	}

	if (renderQueue.IsEmpty() == false)
	{
		renderQueue.Sort();
		ProcessRenderQueue(renderQueue, renderPassType, RenderableType_Opaque);

		frameAllocator.Free(renderQueue.GetBatchCount() * sizeof(RenderBatch));
	}

	mGraphicsDevice->EndEvent();
}

void Renderer::RenderSceneTransparent(std::shared_ptr<CameraComponent> camera, RenderPassType renderPassTypee)
{
	mGraphicsDevice->BeginEvent("RenderSceneTransparent");


	mGraphicsDevice->EndEvent();
}

void Renderer::RenderImpostor(CameraComponent& camera, RenderPassType renderPassType)
{
	mGraphicsDevice->BeginEvent("RenderImpostor");

	mGraphicsDevice->EndEvent();
}

void Renderer::PostprocessTonemap(Texture2D& input, Texture2D& output, F32 exposure)
{
	mGraphicsDevice->BeginEvent("Postprocess_Tonemap");

	BindConstanceBuffer(SHADERSTAGES_CS);

	mGraphicsDevice->BindRenderTarget(0, nullptr, nullptr);
	mGraphicsDevice->BindComputeShader(mShaderLib->GetComputeShader(ComputeShaderType_Tonemapping));

	// bind input texture
	mGraphicsDevice->BindGPUResource(SHADERSTAGES_CS, input, TEXTURE_SLOT_UNIQUE_0);

	// bind post process buffer
	const TextureDesc desc = output.GetDesc();
	PostprocessCB cb;
	cb.gPPResolution.x = desc.mWidth;
	cb.gPPResolution.y = desc.mHeight;
	cb.gPPInverseResolution.x = (1.0f / desc.mWidth);
	cb.gPPInverseResolution.y = (1.0f / desc.mHeight);
	cb.gPPParam1 = exposure;

	GPUBuffer& postprocessBuffer = mBufferManager->GetConstantBuffer(ConstantBufferType_Postprocess);
	mGraphicsDevice->UpdateBuffer(postprocessBuffer, &cb, sizeof(PostprocessCB));
	mGraphicsDevice->BindConstantBuffer(SHADERSTAGES_CS, postprocessBuffer, CB_GETSLOT_NAME(PostprocessCB));

	// bind output texture
	GPUResource* uavs[] = { &output };
	mGraphicsDevice->BindUAVs(uavs, 0, 1);
	mGraphicsDevice->Dispatch(
		(U32)((desc.mWidth + SHADER_POSTPROCESS_BLOCKSIZE - 1) / SHADER_POSTPROCESS_BLOCKSIZE),
		(U32)((desc.mHeight + SHADER_POSTPROCESS_BLOCKSIZE - 1) / SHADER_POSTPROCESS_BLOCKSIZE),
		1
	);
	mGraphicsDevice->UnBindUAVs(0, 1);
	mGraphicsDevice->EndEvent();
}

void Renderer::PostprocessFXAA(Texture2D& input, Texture2D& output)
{
	mGraphicsDevice->BeginEvent("Postprocess_FXAA");

	BindConstanceBuffer(SHADERSTAGES_CS);

	mGraphicsDevice->BindRenderTarget(0, nullptr, nullptr);
	mGraphicsDevice->BindComputeShader(mShaderLib->GetComputeShader(ComputeShaderType_FXAA));

	// bind input texture
	mGraphicsDevice->BindGPUResource(SHADERSTAGES_CS, input, TEXTURE_SLOT_UNIQUE_0);

	// bind post process buffer
	const TextureDesc desc = output.GetDesc();
	PostprocessCB cb;
	cb.gPPResolution.x = desc.mWidth;
	cb.gPPResolution.y = desc.mHeight;
	cb.gPPInverseResolution.x = (1.0f / desc.mWidth);
	cb.gPPInverseResolution.y = (1.0f / desc.mHeight);

	GPUBuffer& postprocessBuffer = mBufferManager->GetConstantBuffer(ConstantBufferType_Postprocess);
	mGraphicsDevice->UpdateBuffer(postprocessBuffer, &cb, sizeof(PostprocessCB));
	mGraphicsDevice->BindConstantBuffer(SHADERSTAGES_CS, postprocessBuffer, CB_GETSLOT_NAME(PostprocessCB));

	// bind output texture
	GPUResource* uavs[] = { &output };
	mGraphicsDevice->BindUAVs(uavs, 0, 1);
	mGraphicsDevice->Dispatch(
		(U32)((desc.mWidth + SHADER_POSTPROCESS_BLOCKSIZE - 1) / SHADER_POSTPROCESS_BLOCKSIZE),
		(U32)((desc.mHeight + SHADER_POSTPROCESS_BLOCKSIZE - 1) / SHADER_POSTPROCESS_BLOCKSIZE),
		1
	);
	mGraphicsDevice->UnBindUAVs(0, 1);
	mGraphicsDevice->EndEvent();
}

void Renderer::BindCommonResource()
{
	SamplerState& linearClampGreater = *mStateManager->GetSamplerState(SamplerStateID_LinearClampGreater);
	SamplerState& anisotropicSampler = *mStateManager->GetSamplerState(SamplerStateID_ANISOTROPIC);
	SamplerState& cmpDepthSampler = *mStateManager->GetSamplerState(SamplerStateID_Comparision_depth);

	for (int stageIndex = 0; stageIndex < SHADERSTAGES_COUNT; stageIndex++)
	{
		SHADERSTAGES stage = static_cast<SHADERSTAGES>(stageIndex);
		mGraphicsDevice->BindSamplerState(stage, linearClampGreater, SAMPLER_LINEAR_CLAMP_SLOT);
		mGraphicsDevice->BindSamplerState(stage, anisotropicSampler, SAMPLER_ANISOTROPIC_SLOT);
		mGraphicsDevice->BindSamplerState(stage, cmpDepthSampler, SAMPLER_COMPARISON_SLOT);
	}

	// bind shader light resource
	GPUResource* resources[] = {
		&mBufferManager->GetStructuredBuffer(StructuredBufferType_ShaderLight),
		&mBufferManager->GetStructuredBuffer(StructuredBufferType_MatrixArray)
	};
	mGraphicsDevice->BindGPUResources(SHADERSTAGES_PS, resources, SBSLOT_SHADER_LIGHT_ARRAY, 2);
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
	frameCB.gFrameGamma = GetGamma();
	frameCB.gFrameShadowCascadeCount = shadowCascadeCount;

	GPUBuffer& frameBuffer = mBufferManager->GetConstantBuffer(ConstantBufferType_Frame);
	mGraphicsDevice->UpdateBuffer(frameBuffer, &frameCB, sizeof(FrameCB));
}

Scene & Renderer::GetMainScene()
{
	return Scene::GetScene();
}

PipelineStateManager & Renderer::GetPipelineStateManager()
{
	return *mPipelineStateManager;
}

Renderer2D & Renderer::GetRenderer2D()
{
	return *mRenderer2D;
}

RenderPath* Renderer::GetRenderPath()
{
	return mCurrentRenderPath.get();
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

void Renderer::AddDeferredTextureMipGen(Texture2D& texture)
{
	if (mDeferredMIPGenerator == nullptr) {
		Debug::Warning("Invalid deferred texture mip generator.");
		return;
	}

	mDeferredMIPGenerator->AddTexture(texture);
}

GraphicsDevice* Renderer::CreateGraphicsDeviceByType(RenderingDeviceType deviceType, HWND window)
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

void Renderer::ProcessRenderQueue(RenderQueue & queue, RenderPassType renderPassType, RenderableType renderableType)
{
	if (queue.IsEmpty() == true) {
		return;
	}

	LinearAllocator& frameAllocator = GetFrameAllocator(FrameAllocatorType_Render);
	auto& scene = GetMainScene();

	size_t instanceSize = sizeof(RenderInstance);
	GraphicsDevice::GPUAllocation instances = mGraphicsDevice->AllocateGPU(queue.GetBatchCount() * instanceSize);

	// ����ʹ����ͬmesh��object������ͬһ��instanceBatch��������ͬ��shader��
	// ������Ȼ��Ϊÿ��object����һ��instance��input layout����������ɫ����������任����

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

			RenderBatchInstance* batchInstance = (RenderBatchInstance*)frameAllocator.Allocate(sizeof(RenderBatchInstance));
			batchInstance->mMeshEntity = meshEntity;
			batchInstance->mDataOffset = instances.offset + index * instanceSize;
			batchInstance->mInstanceCount = 0;

			mRenderBatchInstances.push_back(batchInstance);
		}

		const auto object = scene.GetComponent<ObjectComponent>(objectEntity);
		const auto transform = scene.GetComponent<TransformComponent>(objectEntity);

		// ����ÿ��batch������һ��object����worldMatrix
		// ������shader�м��㶥�����������
		const XMFLOAT4X4 worldMatrix = transform->GetWorldTransform();
		const XMFLOAT4 color = XMConvert(object->mColor) ;
		((RenderInstance*)instances.data)[index].Setup(worldMatrix, color);

		// ��ǰbatchInstance��Ȼ��array���
		mRenderBatchInstances.back()->mInstanceCount++;
	}

	// simpleBindTexture��ֻ��baseColorMap
	bool simpleBindTexture = false;
	if (renderPassType == RenderPassType_Shadow)
	{
		// ��Ӱ��ͼ����ʱֻ��Ҫ��baseColorMap
		simpleBindTexture = true;
	}

	// process render batch instances
	for (auto& bathInstance : mRenderBatchInstances)
	{
		const auto mesh = scene.GetComponent<MeshComponent>(bathInstance->mMeshEntity);

		mGraphicsDevice->BindIndexBuffer(*mesh->GetIndexBuffer(), mesh->GetIndexFormat(), 0);

		BoundVexterBufferType prevBoundType = BoundVexterBufferType_Nothing;
		for (auto& subset : mesh->GetSubsets())
		{
			auto material = scene.GetComponent<MaterialComponent>(subset.mMaterialID);
			if (material == nullptr) {
				continue;
			}

			// TODO: �ܶ���������ÿ������ϵ�PipelineState��
			PipelineState state = mPipelineStateManager->GetNormalPipelineState(renderPassType, *material);  // TODO
			if (state.IsEmpty()) {
				continue;
			}

			bool is_renderable = false;
			if (renderableType == RenderableType_Opaque)
			{
				is_renderable = true;
			}

			if (renderPassType == RenderPassType_Shadow)
			{
				is_renderable &= material->IsCastingShadow();
			}

			if (is_renderable == false)
			{
				continue;
			}
		
			BoundVexterBufferType boundType = BoundVexterBufferType_All;
			if (renderPassType == RenderPassType_Shadow)
			{
				boundType = BoundVexterBufferType_Pos;
			}

			// bind vertex buffer
			if (prevBoundType != boundType)
			{
				prevBoundType = boundType;

				// ���ݲ�ͬ��BoundType���ݲ�ͬ��ʽ��vertexBuffer
				switch (boundType)
				{
				case BoundVexterBufferType_All:
					{
						GPUBuffer* vbs[] = {
							mesh->GetVertexBufferPos(),
							mesh->GetVertexBufferTex(),
							mesh->GetVertexBufferColor(),
							instances.buffer
						};
						U32 strides[] = {
							sizeof(MeshComponent::VertexPosNormalSubset),
							sizeof(MeshComponent::VertexTex),
							sizeof(MeshComponent::VertexColor),
							(U32)instanceSize
						};
						U32 offsets[] = {
							0,
							0,
							0,
							bathInstance->mDataOffset	// ��Ϊ���е�batch����һ��buffer�������ṩoffset
						};
						mGraphicsDevice->BindVertexBuffer(vbs, 0, ARRAYSIZE(vbs), strides, offsets);
					}
					break;
				case BoundVexterBufferType_Pos:
					{
						GPUBuffer* vbs[] = {
							mesh->GetVertexBufferPos(),
							instances.buffer
						};
						U32 strides[] = {
							sizeof(MeshComponent::VertexPosNormalSubset),
							(U32)instanceSize
						};
						U32 offsets[] = {
							0,
							bathInstance->mDataOffset	// ��Ϊ���е�batch����һ��buffer�������ṩoffset
						};
						mGraphicsDevice->BindVertexBuffer(vbs, 0, ARRAYSIZE(vbs), strides, offsets);
					}
					break;

				case BoundVexterBufferType_Pos_Tex:
					{
						GPUBuffer* vbs[] = {
							mesh->GetVertexBufferPos(),
							mesh->GetVertexBufferTex(),
							instances.buffer
						};
						U32 strides[] = {
							sizeof(MeshComponent::VertexPosNormalSubset),
							sizeof(MeshComponent::VertexTex),
							(U32)instanceSize
						};
						U32 offsets[] = {
							0,
							0,
							bathInstance->mDataOffset	// ��Ϊ���е�batch����һ��buffer�������ṩoffset
						};
						mGraphicsDevice->BindVertexBuffer(vbs, 0, ARRAYSIZE(vbs), strides, offsets);
					}
					break;
				}
			}

			// bind material state
			mGraphicsDevice->BindShaderInfoState(state);

			// bind material constant buffer
			mGraphicsDevice->BindConstantBuffer(SHADERSTAGES_VS, material->GetConstantBuffer(), CB_GETSLOT_NAME(MaterialCB));
			mGraphicsDevice->BindConstantBuffer(SHADERSTAGES_PS, material->GetConstantBuffer(), CB_GETSLOT_NAME(MaterialCB));

			// bind material texture
			if (simpleBindTexture)
			{
				GPUResource* resources[] = {
					material->mBaseColorMap.get(),
				};
				mGraphicsDevice->BindGPUResources(SHADERSTAGES_PS, resources, TEXTURE_SLOT_0, 1);
			}
			else
			{
				GPUResource* resources[] = {
					material->mBaseColorMap.get(),
					material->mNormalMap.get(),
					material->mSurfaceMap.get(),
				};
				mGraphicsDevice->BindGPUResources(SHADERSTAGES_PS, resources, TEXTURE_SLOT_0, 3);
			}

			// draw
			mGraphicsDevice->DrawIndexedInstances(subset.mIndexCount, bathInstance->mInstanceCount, subset.mIndexOffset, 0, 0);
		}
	}

	mGraphicsDevice->UnAllocateGPU();	
	frameAllocator.Free(instancedBatchCount * sizeof(RenderBatchInstance));
}

void Renderer::BindConstanceBuffer(SHADERSTAGES stage)
{
	GPUBuffer& cameraBuffer = mBufferManager->GetConstantBuffer(ConstantBufferType_Camera);
	mGraphicsDevice->BindConstantBuffer(stage, cameraBuffer, CB_GETSLOT_NAME(CameraCB));

	GPUBuffer& frameBuffer = mBufferManager->GetConstantBuffer(ConstantBufferType_Frame);
	mGraphicsDevice->BindConstantBuffer(stage, frameBuffer, CB_GETSLOT_NAME(FrameCB));
}

void Renderer::BindShadowMaps(SHADERSTAGES stage)
{
	mGraphicsDevice->BindGPUResource(SHADERSTAGES_PS, shadowMapArray2D, TEXTURE_SLOT_SHADOW_ARRAY_2D);
}

void Renderer::RenderDirLightShadowmap(LightComponent& light, CameraComponent& camera)
{
	// ����CSM�ķ������Ӱ
	// 1.����׶��ֳ�CASCADE_COUNT������׶�壬����ÿ������׶�崴����Χ��ͱ任����
	std::vector<CascadeShadowCamera> cascadeShadowCameras;
	CreateCascadeShadowCameras(light, camera, shadowCascadeCount, cascadeShadowCameras);

	LinearAllocator& frameAllocator = GetFrameAllocator(FrameAllocatorType_Render);
	Scene& mainScene = GetMainScene();
	GraphicsDevice& device = GetDevice();
	GPUBuffer& cameraBuffer = mBufferManager->GetConstantBuffer(ConstantBufferType_Camera);

	ViewPort vp;
	vp.mWidth = (F32)shadowRes2DResolution;
	vp.mHeight = (F32)shadowRes2DResolution;
	vp.mMinDepth = 0.0f;
	vp.mMaxDepth = 1.0f;
	device.BindViewports(&vp, 1, GraphicsThread::GraphicsThread_IMMEDIATE);

	// 2.���ݼ�����׶�崴���İ�Χ�У���ȡÿ��������Ӧ������
	for (U32 cascadeLevel = 0; cascadeLevel < shadowCascadeCount; cascadeLevel++)
	{
		CascadeShadowCamera& cam = cascadeShadowCameras[cascadeLevel];
		RenderQueue renderQueue;

		for (U32 index = 0; index < mainScene.mObjectAABBs.GetCount(); index++)
		{
			auto aabb = mainScene.mObjectAABBs[index];
			if (cam.mBoundingBox.Intersects(aabb->GetAABB()))
			{
				auto object = mainScene.mObjects[index];
				if (object == nullptr || 
					object->IsRenderable() == false ||
					object->IsCastShadow() == false) 
				{
					continue;
				}

				RenderBatch* renderBatch = (RenderBatch*)frameAllocator.Allocate(sizeof(RenderBatch));
				renderBatch->Init(object->GetCurrentEntity(), object->mMeshID);
				renderQueue.AddBatch(renderBatch);
			}
		}

		// ����ÿ���������ư�������������ͼ
		if (!renderQueue.IsEmpty())
		{
			U32 resourceIndex = light.GetShadowMapIndex() + cascadeLevel;

			CameraCB cb;
			DirectX::XMStoreFloat4x4(&cb.gCameraVP, cam.GetViewProjectionMatrix());
			device.UpdateBuffer(cameraBuffer, &cb, sizeof(CameraCB));

			device.ClearDepthStencil(shadowMapArray2D, CLEAR_DEPTH, 0.0f, 0, resourceIndex);
			device.BindRenderTarget(0, nullptr, &shadowMapArray2D, resourceIndex);
			ProcessRenderQueue(renderQueue, RenderPassType_Shadow, RenderableType_Opaque);

			frameAllocator.Free(renderQueue.GetBatchCount() * sizeof(RenderBatch));
		}
	}
}

LinearAllocator& Renderer::GetFrameAllocator(FrameAllocatorType type)
{
	LinearAllocator& allocator = mFrameAllocator[static_cast<U32>(type)];

	// ��ʱֻ��һ��frameAllocator
	// �������ڻ�ȡʱ���Ż�ȥ�����ڴ�
	if (allocator.GetCapacity() <= 0) {
		allocator.Reserve((size_t)MAX_FRAME_ALLOCATOR_SIZE);
	}

	return allocator;
}

void Renderer::FrameCullings::Clear()
{
	mCulledObjects.clear();
	mCulledLights.clear();
}


}