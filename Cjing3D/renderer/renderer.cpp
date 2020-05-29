#include "renderer.h"
#include "shaderLib.h"
#include "stateManager.h"
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
#include "pipelineStates\pipelineStateManager.h"

#include "passes\renderPass.h"
#include "passes\terrainPass.h"

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
	std::vector<F32> currentCascadeSplits = {
		0.0f,
		0.01f,
		0.1f,
		1.0f
	};

	Texture2D shadowMapArray2D;
	std::vector<RenderBehavior> shadowRenderBehaviors;
	std::vector<bool> shadowMapDirtyTable;

	// ��������׶��
	struct LightShadowCamera
	{
		Frustum mFrustum;
		XMMATRIX mViewProjection;

		LightShadowCamera() {}
		LightShadowCamera(const XMVECTOR& eyePos, const XMVECTOR& rot, F32 nearZ, F32 farZ, F32 fov)
		{
			const XMMATRIX matRot = XMMatrixRotationQuaternion(rot);
			// create light camera view matrix
			// light default dir is (0, -1, 0), up(0, 0, 1)
			const XMVECTOR to = XMVector3TransformNormal(XMVectorSet(0.0f,-1.0f, 0.0f, 0.0f), matRot);
			const XMVECTOR up = XMVector3TransformNormal(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), matRot);
			const XMMATRIX view = XMMatrixLookToLH(eyePos, to, up);
			const XMMATRIX proj = XMMatrixPerspectiveFovLH(fov, 1.0f, farZ, nearZ);	// reversed depth buffer!!!
			mViewProjection = view * proj;
			mFrustum.SetupFrustum(mViewProjection);
		}

		XMMATRIX GetViewProjectionMatrix()
		{
			return mViewProjection;
		}
	};

	// shadow cubemap
	U32 shadowResCubeResolution = 512;
	U32 shadowMapCubeCount = 5;
	Texture2D shadowMapArrayCube;
	std::vector<bool> shadowCubeMapDirtyTable;
	std::vector<RenderBehavior> shadowCubeRenderBehaviors;

	void ClearShadowMap()
	{
		shadowMapArray2D.Clear();
		shadowMapArrayCube.Clear();
	}

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

		shadowMapDirtyTable.resize(count, false);
		shadowRenderBehaviors.resize(count);
		for (U32 i = 0; i < count; i++) 
		{
			I32 subresourceIndex = device.CreateDepthStencilView(shadowMapArray2D, i, 1);

			RenderBehaviorDesc desc = {};
			desc.mParams.push_back({ RenderBehaviorParam::RenderType_DepthStencil, &shadowMapArray2D, subresourceIndex, RenderBehaviorParam::RenderOperation_Clear });
			device.CreateRenderBehavior(desc, shadowRenderBehaviors[i]);
		}
	}

	void SetShadowMapCubeProperty(Renderer& renderer, U32 resolution, U32 count)
	{
		shadowResCubeResolution = resolution;
		shadowMapCubeCount = count;

		if (resolution <= 0 || count <= 0) {
			return;
		}

		TextureDesc desc = {};
		desc.mWidth = resolution;
		desc.mHeight = resolution;
		desc.mArraySize = count * 6;
		desc.mMipLevels = 1;
		desc.mFormat = FORMAT_R16_TYPELESS;
		desc.mBindFlags = BIND_DEPTH_STENCIL | BIND_SHADER_RESOURCE;
		desc.mUsage = USAGE_DEFAULT;
		desc.mMiscFlags = RESOURCE_MISC_TEXTURECUBE;

		GraphicsDevice& device = renderer.GetDevice();
		const auto result = device.CreateTexture2D(&desc, nullptr, shadowMapArrayCube);
		Debug::ThrowIfFailed(result, "Failed to create shadow cube map:%08x", result);
		device.SetResourceName(shadowMapArrayCube, "ShadowMapArrayCube");

		shadowCubeMapDirtyTable.resize(count, false);
		shadowCubeRenderBehaviors.resize(count);
		for (U32 i = 0; i < count; i++)
		{
			I32 subresourceIndex = device.CreateDepthStencilView(shadowMapArrayCube, i * 6, 6);

			RenderBehaviorDesc desc = {};
			desc.mParams.push_back({ RenderBehaviorParam::RenderType_DepthStencil, &shadowMapArrayCube, subresourceIndex, RenderBehaviorParam::RenderOperation_Clear });
			device.CreateRenderBehavior(desc, shadowCubeRenderBehaviors[i]);
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

	void CreateCascadeShadowCameras(LightComponent& light, CameraComponent& camera, U32 cascadeCount, std::vector<LightShadowCamera>& csfs)
	{
		if (currentCascadeSplits.size() != (size_t)(cascadeCount + 1)) {
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

			XMVECTOR vRadius = XMVectorReplicate(-radius);
			XMVECTOR vMin = center - vRadius;
			XMVECTOR vMax = center + vRadius;

			// ���뵽����������
			const XMVECTOR texelSize = XMVectorSubtract(vMax, vMin) / (F32)shadowRes2DResolution;
			vMin = XMVectorFloor(vMin / texelSize) * texelSize;
			vMax = XMVectorFloor(vMax / texelSize) * texelSize;
			XMVECTOR vCenter = (vMax + vMax) * 0.5f;
			
			// ����׶�屾���Χ����z�᷽���С�������Χ���԰�����������壨������׶��֮������壬���ڹ�Դ������Ҳ���ܲ�����Ӱ��
			XMFLOAT3 _min, _max, _center;
			XMStoreFloat3(&_min, vMin);
			XMStoreFloat3(&_max, vMax);
			XMStoreFloat3(&_center, vCenter);

			float ext = std::max(abs(_center.z - _min.z), farPlane * 0.5f);
			_min.z = _center.z - ext;
			_max.z = _center.z + ext;

			auto& cam = csfs.emplace_back();

			// light view and projection matrix, reversed z
			const XMMATRIX lightProjection = XMMatrixOrthographicOffCenterLH(_min.x, _max.x, _min.y, _max.y, _max.z, _min.z); 
			cam.mViewProjection = lightView * lightProjection;
			cam.mFrustum.SetupFrustum(cam.mViewProjection);
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
	GetCamera().SetupPerspective((F32)mScreenSize[0], (F32)mScreenSize[1], 0.1f, 1000.0f);
	GetCamera().SetCameraStatus(
		{ 0.0f, 0.0f, -50.0f },
		{ 0.0f, 0.0f,   1.0f },
		{ 0.0f, 1.0f,   0.0f }
	);

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
	mFrameCullings[&GetCamera()] = FrameCullings();

	mFrameData.mFrameScreenSize = { (F32)mScreenSize[0], (F32)mScreenSize[1] };
	mFrameData.mFrameAmbient = { 0.1f, 0.1f, 0.1f };

	SetShadowMap2DProperty(*this, shadowRes2DResolution, shadowMap2DCount);
	SetShadowMapCubeProperty(*this, shadowResCubeResolution, shadowMapCubeCount);

	// initialize render pass
	InitializeRenderPasses();

	mIsInitialized = true;
}

void Renderer::Uninitialize()
{
	if (mIsInitialized == false) {
		return;
	}

	ClearShadowMap();
	UninitializeRenderPasses();

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

	GetFrameAllocator(FrameAllocatorType_Render).FreeBuffer();

	mIsInitialized = false;
}

void Renderer::FixedUpdate()
{
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

	CameraComponent& mainCamera = GetCamera();
	mainCamera.Update();

	// update materials
	mPendingUpdateMaterials.clear();

	auto& materials = mainScene.mMaterials;
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
		const CameraComponent* camera = kvp.first;
		if (camera == nullptr && camera != &mainCamera) {
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
			U32 currentShadowMapCubeCount = 0;
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
				case LightComponent::LightType_Directional:
					if ((currentShadowMap2DCount + shadowCascadeCount) <= shadowMap2DCount)
					{
						light->SetShadowMapIndex(currentShadowMap2DCount);
						currentShadowMap2DCount += shadowCascadeCount;
					}
					break;
				case LightComponent::LightType_Point:
					if (currentShadowMapCubeCount < shadowMapCubeCount)
					{
						light->SetShadowMapIndex(currentShadowMapCubeCount);
						currentShadowMapCubeCount++;
					}
					break;
				default:
					break;
				}
			}

			frameAllocator.Free(culledLightCount * sizeof(U32));
		}
	}

	// refresh render pass 
	for (auto kvp : mRenderPassMap)
	{
		auto renderPass = kvp.second;
		if (renderPass != nullptr) {
			renderPass->UpdatePerFrameData(deltaTime);
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

	CameraComponent& mainCamera = GetCamera();

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

	XMMATRIX viewMatrix = mainCamera.GetViewMatrix();
	FrameCullings& frameCulling = mFrameCullings.at(&mainCamera);
	auto& lights = frameCulling.mCulledLights;
	for (const auto& lightIndex : lights)
	{
		if (lightCount >= SHADER_LIGHT_COUNT) {
			Debug::Warning("Invalid light count" + std::to_string(lightCount) + " " + std::to_string(SHADER_LIGHT_COUNT));
			break;
		}

		LightComponent* light = mainScene.GetComponentByIndex<LightComponent>(lightIndex);
		if (light == nullptr) {
			continue;
		}

		ShaderLight shaderLight = light->CreateShaderLight(viewMatrix);

		// ���light֧����Ӱ�����ѷ�����shadowMapIndex���򴫵ݶ�Ӧ
		// shader viewProjection Matrix��shaderMapIndex
		if (light->IsCastShadow() && light->GetShadowMapIndex() >= 0)
		{
			shaderLight.shadowKernel = (1.0f / shadowRes2DResolution);
			shaderLight.SetShadowData(currentMatrixIndex, light->GetShadowMapIndex());
			switch (light->GetLightType())
			{
			case Cjing3D::LightComponent::LightType_Directional:
			{
				// TODO:��renderShadow�����ּ�����һ�Ρ���
				std::vector<LightShadowCamera> cascadeShadowCameras;
				CreateCascadeShadowCameras(*light, GetCamera(), shadowCascadeCount, cascadeShadowCameras);

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

	// mesh skinning
	ProcessSkinning();

	// refresh render pass 
	for (auto kvp : mRenderPassMap)
	{
		auto renderPass = kvp.second;
		if (renderPass != nullptr) {
			renderPass->RefreshRenderData();
		}
	}
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

	// end frame 
	GetFrameAllocator(FrameAllocatorType_Render).Reset();
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

CameraComponent& Renderer::GetCamera()
{
	static CameraComponent mCamera;
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

void Renderer::SetAlphaCutRef(F32 alpha)
{
	F32 newAlphaRef = 1.0f - alpha + 1.0f / 256.0f; // ����һ����λ(1.0f/256.f)ʹ���ƫ��
	if (newAlphaRef != mCommonCB.gCommonAlphaCutRef)
	{
		mCommonCB.gCommonAlphaCutRef = newAlphaRef;
		mGraphicsDevice->UpdateBuffer(mBufferManager->GetConstantBuffer(ConstantBufferType_Common), &mCommonCB, sizeof(CommonCB));
	}
}

void Renderer::RenderShadowmaps(CameraComponent& camera)
{
	FrameCullings& frameCulling = mFrameCullings[&camera];
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
				RenderDirLightShadowmap(*light, camera);
				break;
			case Cjing3D::LightComponent::LightType_Point:
				RenderPointLightShadowmap(*light, camera);
				break;
			default:
				break;
			}
		}
	}

	mGraphicsDevice->BindRenderTarget(0, nullptr, nullptr);
	mGraphicsDevice->EndEvent();
}

void Renderer::RenderSceneOpaque(CameraComponent& camera, RenderPassType renderPassType)
{
	mGraphicsDevice->BeginEvent("RenderScene");

	BindShadowMaps(SHADERSTAGES_PS);

	// terrain render
	GetRenderPass(STRING_ID(TerrainPass))->Render();

	// impostor render
	RenderImpostor(camera, renderPassType);

	LinearAllocator& frameAllocator = GetFrameAllocator(FrameAllocatorType_Render);
	auto& scene = GetMainScene();

	F32x3 cameraPos = camera.GetCameraPos();
	// opaque object render
	RenderQueue renderQueue;
	FrameCullings& frameCulling = mFrameCullings[&camera];
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
			continue;
		}

		RenderBatch* renderBatch = (RenderBatch*)frameAllocator.Allocate(sizeof(RenderBatch));
		renderBatch->Init(objectEntity, object->mMeshID, DistanceEstimated(cameraPos, object->mCenter));

		renderQueue.AddBatch(renderBatch);
	}

	if (renderQueue.IsEmpty() == false)
	{
		renderQueue.Sort(RenderQueue::FrontToBack);
		ProcessRenderQueue(renderQueue, renderPassType, RenderableType_Opaque);

		frameAllocator.Free(renderQueue.GetBatchCount() * sizeof(RenderBatch));
	}

	mGraphicsDevice->EndEvent();
}

void Renderer::RenderSceneTransparent(CameraComponent& camera, RenderPassType renderPassType)
{
	mGraphicsDevice->BeginEvent("RenderSceneTransparent");

	BindShadowMaps(SHADERSTAGES_PS);

	LinearAllocator& frameAllocator = GetFrameAllocator(FrameAllocatorType_Render);
	auto& scene = GetMainScene();

	F32x3 cameraPos = camera.GetCameraPos();
	// transparent object render
	RenderQueue renderQueue;
	FrameCullings& frameCulling = mFrameCullings[&camera];
	auto& objects = frameCulling.mCulledObjects;
	for (const auto& objectIndex : objects)
	{
		auto objectEntity = scene.GetEntityByIndex<ObjectComponent>(objectIndex);
		auto object = scene.GetComponent<ObjectComponent>(objectEntity);

		if (object == nullptr ||
			object->GetObjectType() != ObjectComponent::OjbectType_Renderable ||
			object->GetRenderableType() != RenderableType_Transparent) {
			continue;
		}

		RenderBatch* renderBatch = (RenderBatch*)frameAllocator.Allocate(sizeof(RenderBatch));
		renderBatch->Init(objectEntity, object->mMeshID, DistanceEstimated(cameraPos, object->mCenter));
		renderQueue.AddBatch(renderBatch);
	}

	if (renderQueue.IsEmpty() == false)
	{
		// ��Ⱦ͸������ʱ��Ҫ��Զ����������Ⱦ
		renderQueue.Sort(RenderQueue::BackToFront);
		ProcessRenderQueue(renderQueue, renderPassType, RenderableType_Transparent);

		frameAllocator.Free(renderQueue.GetBatchCount() * sizeof(RenderBatch));
	}

	mGraphicsDevice->EndEvent();
}

void Renderer::RenderImpostor(CameraComponent& camera, RenderPassType renderPassType)
{
	mGraphicsDevice->BeginEvent("RenderImpostor");

	mGraphicsDevice->EndEvent();
}

void Renderer::RenderSky()
{
	mGraphicsDevice->BeginEvent("RenderSky");

	auto& scene = GetMainScene();
	if (scene.mWeathers.GetCount() > 0)
	{
		auto currentWeather = scene.mWeathers.GetComponentByIndex(0);
		if (currentWeather->mSkyMap != nullptr)
		{
			PipelineState pso = mPipelineStateManager->GetPipelineStateByID(PipelineStateID_SkyRendering);
			if (pso.IsValid())
			{
				mGraphicsDevice->BindPipelineState(pso);
				mGraphicsDevice->BindGPUResource(SHADERSTAGES_PS, *currentWeather->mSkyMap, TEXTURE_SLOT_GLOBAL_ENV_MAP);
			}
		}

		mGraphicsDevice->Draw(3, 0);
	}

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

	DirectX::XMStoreFloat4x4(&cb.gCameraVP,    camera.GetViewProjectionMatrix());
	DirectX::XMStoreFloat4x4(&cb.gCameraView,  camera.GetViewMatrix());
	DirectX::XMStoreFloat4x4(&cb.gCameraProj,  camera.GetProjectionMatrix());
	DirectX::XMStoreFloat4x4(&cb.gCameraInvVP, camera.GetInvViewProjectionMatrix());

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

void Renderer::RegisterRenderPass(const StringID& name, std::shared_ptr<RenderPass> renderPath)
{
	mRenderPassMap[name] = renderPath; 
}

std::shared_ptr<RenderPass> Renderer::GetRenderPass(const StringID& name)
{
	auto it = mRenderPassMap.find(name);
	if (it != mRenderPassMap.end()) {
		return it->second;
	}
	return nullptr;
}

TerrainTreePtr Renderer::RegisterTerrain(ECS::Entity entity, U32 width, U32 height, U32 elevation)
{
	auto renderPass = GetRenderPass(STRING_ID(TerrainPass));
	if (renderPass == nullptr)
	{
		Debug::Warning("Terrain pass is not ready.");
		return nullptr;
	}

	TerrainPass& terrainPass = dynamic_cast<TerrainPass&>(*renderPass);
	return terrainPass.RegisterTerrain(entity, width, height, elevation);
}

void Renderer::UnRegisterTerrain(ECS::Entity entity)
{
	auto renderPass = GetRenderPass(STRING_ID(TerrainPass));
	if (renderPass == nullptr)
	{
		Debug::Warning("Terrain pass is not ready.");
		return;
	}

	TerrainPass& terrainPass = dynamic_cast<TerrainPass&>(*renderPass);
	terrainPass.UnRegisterTerrain(entity);
}

TerrainTreePtr Renderer::GetTerrainTree(ECS::Entity entity)
{
	auto renderPass = GetRenderPass(STRING_ID(TerrainPass));
	if (renderPass == nullptr)
	{
		Debug::Warning("Terrain pass is not ready.");
		return nullptr;
	}

	TerrainPass& terrainPass = dynamic_cast<TerrainPass&>(*renderPass);
	return terrainPass.GetTerrainTree(entity);
}

ShaderPtr Renderer::LoadShader(SHADERSTAGES stages, const std::string& path)
{
	return mShaderLib->LoadShader(stages, path);
}

VertexShaderInfo Renderer::LoadVertexShaderInfo(const std::string& path, VertexLayoutDesc* desc, U32 numElements)
{
	return mShaderLib->LoadVertexShaderInfo(path, desc, numElements);
}

void Renderer::InitializeRenderPasses()
{
	std::shared_ptr<RenderPass> terrainPass = std::shared_ptr<RenderPass>(new TerrainPass(*this));
	terrainPass->Initialize();
	RegisterRenderPass(STRING_ID(TerrainPass), terrainPass);
}

void Renderer::UninitializeRenderPasses()
{
	for (auto kvp : mRenderPassMap)
	{
		auto renderPass = kvp.second;
		if (renderPass != nullptr) {
			renderPass->Uninitialize();
		}
	}
	mRenderPassMap.clear();
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

void Renderer::ProcessRenderQueue(RenderQueue & queue, RenderPassType renderPassType, RenderableType renderableType, U32 instanceReplicator, InstanceHandler* instanceHandler)
{
	if (queue.IsEmpty() == true) {
		return;
	}

	LinearAllocator& frameAllocator = GetFrameAllocator(FrameAllocatorType_Render);
	auto& scene = GetMainScene();

	size_t instanceSize = sizeof(RenderInstance);
	size_t allocSize = queue.GetBatchCount() * instanceReplicator * instanceSize;
	GraphicsDevice::GPUAllocation instances = mGraphicsDevice->AllocateGPU(allocSize);

	// ����ʹ����ͬmesh��object������ͬһ��instanceBatch��������ͬ��shader��
	// ������Ȼ��Ϊÿ��object����һ��instance��input layout����������ɫ����������任����

	ECS::Entity prevMeshEntity = ECS::INVALID_ENTITY;
	U32 instancedBatchCount = 0;

	std::vector<RenderBatchInstance*> mRenderBatchInstances;

	auto& renderBatches = queue.GetRenderBatches();
	size_t batchesCount = renderBatches.size();
	size_t currentInstanceCount = 0;
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

		const auto object = scene.mObjects.GetComponent(objectEntity);
		const auto transform = scene.mTransforms.GetComponent(objectEntity);
		const auto aabb = scene.mObjectAABBs.GetComponent(objectEntity);

		// ����ÿ��batch������һ��object����worldMatrix
		// ������shader�м��㶥�����������
		const XMFLOAT4X4 worldMatrix = transform->GetWorldTransform();
		const XMFLOAT4 color = XMConvert(object->mColor) ;

		for (U32 subIndex = 0; subIndex < instanceReplicator; subIndex++)
		{
			// check condition
			if (instanceHandler != nullptr && 
				instanceHandler->checkCondition_ != nullptr && 
				!instanceHandler->checkCondition_(subIndex, objectEntity, scene)) 
			{
				continue;
			}

			((RenderInstance*)instances.data)[currentInstanceCount].Setup(worldMatrix, color);

			// set userdata
			if (instanceHandler != nullptr &&
				instanceHandler->processInstance_ != nullptr) 
			{
				instanceHandler->processInstance_(subIndex, ((RenderInstance*)instances.data)[currentInstanceCount]);
			}

			// ��ǰbatchInstance��Ȼ��array���
			mRenderBatchInstances.back()->mInstanceCount++;
			currentInstanceCount++;
		}
	}

	// simpleBindTexture��ֻ��baseColorMap
	bool simpleBindTexture = false;
	if (renderPassType == RenderPassType_Shadow ||
		renderPassType == RenderPassType_Depth ||
		renderPassType == RenderPassType_ShadowCube)
	{
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
			if (!state.IsValid()) {
				continue;
			}

			bool is_renderable = false;
			if (renderableType == RenderableType_Opaque)
			{
				is_renderable = is_renderable || (!material->IsTransparent());
			}
			if (renderableType == RenderableType_Transparent)
			{
				is_renderable = is_renderable || (material->IsTransparent());
			}
			if (renderPassType == RenderPassType_Shadow ||
				renderPassType == RenderPassType_ShadowCube)
			{
				is_renderable &= material->IsCastingShadow();
			}

			if (is_renderable == false)
			{
				continue;
			}
		
			BoundVexterBufferType boundType = BoundVexterBufferType_All;
			if (renderPassType == RenderPassType_Shadow ||
				renderPassType == RenderPassType_ShadowCube)
			{
				boundType = BoundVexterBufferType_Pos;
			}
			else if (renderPassType == RenderPassType_Depth)
			{
				boundType = material->IsNeedAlphaTest() ? BoundVexterBufferType_Pos_Tex : BoundVexterBufferType_Pos;
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
							mesh->GetFinalVertexBufferPos(),
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
							mesh->GetFinalVertexBufferPos(),
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
							mesh->GetFinalVertexBufferPos(),
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

			// set alpha cut ref
			SetAlphaCutRef(material->GetAlphaCutRef());

			// bind material state
			mGraphicsDevice->BindPipelineState(state);

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

	// remove alpha test
	ResetAlphaCutRef();

	mGraphicsDevice->UnAllocateGPU();	
	frameAllocator.Free(instancedBatchCount * sizeof(RenderBatchInstance));
}

void Renderer::BindConstanceBuffer(SHADERSTAGES stage)
{
	mGraphicsDevice->BindConstantBuffer(stage, mBufferManager->GetConstantBuffer(ConstantBufferType_Common), CB_GETSLOT_NAME(CommonCB));
	mGraphicsDevice->BindConstantBuffer(stage, mBufferManager->GetConstantBuffer(ConstantBufferType_Camera), CB_GETSLOT_NAME(CameraCB));
	mGraphicsDevice->BindConstantBuffer(stage, mBufferManager->GetConstantBuffer(ConstantBufferType_Frame),  CB_GETSLOT_NAME(FrameCB));
}

void Renderer::BindShadowMaps(SHADERSTAGES stage)
{
	mGraphicsDevice->BindGPUResource(stage, shadowMapArray2D,   TEXTURE_SLOT_SHADOW_ARRAY_2D);
	mGraphicsDevice->BindGPUResource(stage, shadowMapArrayCube, TEXTURE_SLOT_SHADOW_ARRAY_CUBE);
}

void Renderer::RenderDirLightShadowmap(LightComponent& light, CameraComponent& camera)
{
	// ����CSM�ķ������Ӱ
	// 1.����׶��ֳ�CASCADE_COUNT������׶�壬����ÿ������׶�崴����Χ��ͱ任����
	std::vector<LightShadowCamera> cascadeShadowCameras;
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
		LightShadowCamera& cam = cascadeShadowCameras[cascadeLevel];
		RenderQueue renderQueue;

		for (U32 index = 0; index < mainScene.mObjectAABBs.GetCount(); index++)
		{
			auto aabb = mainScene.mObjectAABBs[index];
			if (cam.mFrustum.Overlaps(aabb->GetAABB()))
			{
				auto object = mainScene.mObjects[index];
				if (object == nullptr || 
					object->IsRenderable() == false ||
					object->IsCastShadow() == false) 
				{
					continue;
				}

				RenderBatch* renderBatch = (RenderBatch*)frameAllocator.Allocate(sizeof(RenderBatch));
				renderBatch->Init(object->GetCurrentEntity(), object->mMeshID, 0);
				renderQueue.AddBatch(renderBatch);
			}
		}

		U32 resourceIndex = light.GetShadowMapIndex() + cascadeLevel;

		// ����ÿ���������ư�������������ͼ
		if (!renderQueue.IsEmpty())
		{
			shadowMapDirtyTable[resourceIndex] = true;

			CameraCB cb;
			DirectX::XMStoreFloat4x4(&cb.gCameraVP, cam.GetViewProjectionMatrix());
			device.UpdateBuffer(cameraBuffer, &cb, sizeof(CameraCB));

			device.BeginRenderBehavior(shadowRenderBehaviors[resourceIndex]);
			ProcessRenderQueue(renderQueue, RenderPassType_Shadow, RenderableType_Opaque);
			device.EndRenderBehavior();

			frameAllocator.Free(renderQueue.GetBatchCount() * sizeof(RenderBatch));
		}
		else if (shadowMapDirtyTable[resourceIndex])
		{
			shadowMapDirtyTable[resourceIndex] = false;
			device.ClearDepthStencil(shadowMapArray2D, CLEAR_DEPTH, 0.0f, 0, resourceIndex);
		}
	}
}

void Renderer::RenderPointLightShadowmap(LightComponent& light, CameraComponent& camera)
{
	LinearAllocator& frameAllocator = GetFrameAllocator(FrameAllocatorType_Render);
	RenderQueue renderQueue;

	// ���Դ���ݷ�Χ������״��Χ��
	Sphere lightSphere(XMConvert(light.mPosition), light.mRange);
	Scene& mainScene = GetMainScene();
	for (U32 index = 0; index < mainScene.mObjectAABBs.GetCount(); index++)
	{
		auto aabb = mainScene.mObjectAABBs[index];
		if (lightSphere.Intersects(aabb->GetAABB()))
		{
			auto object = mainScene.mObjects[index];
			if (object == nullptr ||
				object->IsRenderable() == false ||
				object->IsCastShadow() == false)
			{
				continue;
			}

			RenderBatch* renderBatch = (RenderBatch*)frameAllocator.Allocate(sizeof(RenderBatch));
			renderBatch->Init(object->GetCurrentEntity(), object->mMeshID, 0);
			renderQueue.AddBatch(renderBatch);
		}
	}

	GraphicsDevice& device = GetDevice();
	I32 resourceIndex = light.GetShadowMapIndex();
	if (!renderQueue.IsEmpty())
	{
		shadowCubeMapDirtyTable[resourceIndex] = true;

		const XMVECTOR lightPos = XMLoad(light.mPosition);
		const F32 nearZ = 0.1f;
		const F32 farZ = std::max(1.0f, light.mRange);

		// ����6����Ĺ�Դ����任ͶӰ����˳��Ϊx,-x,y, -y, z,-z
		// light default dir is (0, -1, 0), up(0, 0, 1)
		// q.x = sin(theta / 2) * axis.x
		// q.y = sin(theta / 2) * axis.y
		// q.z = sin(theta / 2) * axis.z
		// q.w = cos(theta / 2)
		// ����ͨ��ŷ����ת���õ���Ԫ��
		const LightShadowCamera cams[] = {
			LightShadowCamera(lightPos, XMVectorSet(0.5f, -0.5f, -0.5f, -0.5f), nearZ, farZ, XM_PIDIV2), //  x
			LightShadowCamera(lightPos, XMVectorSet(0.5f, 0.5f, 0.5f, -0.5f), nearZ, farZ, XM_PIDIV2),   // -x
			LightShadowCamera(lightPos, XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), nearZ, farZ, XM_PIDIV2),    //  y
			LightShadowCamera(lightPos, XMVectorSet(0.0f, 0.0f, 0.0f, -1.0f), nearZ, farZ, XM_PIDIV2),   // -y
			LightShadowCamera(lightPos, XMVectorSet(0.707f, 0, 0, -0.707f), nearZ, farZ, XM_PIDIV2),     //  z, 0.707 = ����2/2
			LightShadowCamera(lightPos, XMVectorSet(0, 0.707f, 0.707f, 0), nearZ, farZ, XM_PIDIV2),      // -z
		};
		CubeMapCB cb;
		for (int i = 0; i < ARRAYSIZE(cams); i++) {
			XMStoreFloat4x4(&cb.gCubeMapVP[i], cams[i].mViewProjection);
		}
		auto buffer = mBufferManager->GetConstantBuffer(ConstantBufferType_CubeMap);
		mGraphicsDevice->UpdateBuffer(buffer, &cb, sizeof(CubeMapCB));
		mGraphicsDevice->BindConstantBuffer(SHADERSTAGES_VS, buffer, CB_GETSLOT_NAME(CubeMapCB));

		ViewPort vp;
		vp.mWidth = (F32)shadowResCubeResolution;
		vp.mHeight = (F32)shadowResCubeResolution;
		vp.mMinDepth = 0.0f;
		vp.mMaxDepth = 1.0f;
		device.BindViewports(&vp, 1, GraphicsThread::GraphicsThread_IMMEDIATE);

		// instance handler
		InstanceHandler instanceHandler;
		instanceHandler.checkCondition_ = [cams](U32 subIndex, ECS::Entity entity, Scene& scene) 
		{
			auto aabb = scene.mObjectAABBs.GetComponent(entity);
			if (aabb == nullptr) {
				return false;
			}
			return cams[subIndex].mFrustum.Overlaps(aabb->GetAABB());
		};
		instanceHandler.processInstance_ = [cams](U32 subIndex, RenderInstance& instance) 
		{
			instance.userdata.x = (F32)subIndex;
		};

		// rendering
		device.BeginRenderBehavior(shadowCubeRenderBehaviors[resourceIndex]);
		ProcessRenderQueue(renderQueue, RenderPassType_ShadowCube, RenderableType_Opaque, 6, &instanceHandler);
		device.EndRenderBehavior();

		frameAllocator.Free(renderQueue.GetBatchCount() * sizeof(RenderBatch));
	}
	else if (shadowCubeMapDirtyTable[resourceIndex])
	{
		shadowCubeMapDirtyTable[resourceIndex] = false;
		device.ClearDepthStencil(shadowMapArrayCube, CLEAR_DEPTH, 0.0f, 0, resourceIndex);
	}
}

void Renderer::ProcessSkinning()
{
	mGraphicsDevice->BeginEvent("Skinning");
	auto& mainScene = GetMainScene();

	bool isResourceSetup = false;
	for (int i = 0; i < mainScene.mMeshes.GetCount(); i++)
	{
		MeshComponent& mesh = *mainScene.mMeshes.GetComponentByIndex(i);
		ECS::Entity entity = mesh.GetCurrentEntity();

		if (!mesh.IsSkinned()) {
			continue;
		}

		auto armature = mainScene.mArmatures.GetComponent(mesh.mArmature);
		if (armature == nullptr) {
			continue;
		}

		if (!isResourceSetup)
		{
			isResourceSetup = true;
			// ��Ϊ��Ҫд�뵽���㻺���У�������Ҫ�����VertexBuffer
			mGraphicsDevice->ClearVertexBuffer();
			mGraphicsDevice->BindComputeShader(mShaderLib->GetComputeShader(ComputeShaderType_Skinning));
		}

		// update bonePoses buffer
		mGraphicsDevice->UpdateBuffer(armature->mBufferBonePoses, armature->mBonePoses.data(), sizeof(ArmatureComponent::BonePose) * armature->mBonePoses.size());
		mGraphicsDevice->BindGPUResource(SHADERSTAGES_CS, armature->mBufferBonePoses, SKINNING_SBSLOT_BONE_POSE_BUFFER);

		GPUResource* resoureces[] = { 
			mesh.GetVertexBufferPos(),
			mesh.GetVertexBufferBoneIndexWeight(),
		};
		GPUResource* uavs[] = {
			mesh.GetVertexBufferStreamoutPos()
		};
		mGraphicsDevice->BindGPUResources(SHADERSTAGES_CS, resoureces, SKINNING_SLOT_VERTEX_POS, ARRAYSIZE(resoureces));
		mGraphicsDevice->BindUAVs(uavs, 0, 1);
		mGraphicsDevice->Dispatch(
			(U32)((mesh.mVertexPositions.size() + SHADER_SKINNING_BLOCKSIZE - 1) / SHADER_SKINNING_BLOCKSIZE),
			1,
			1
		);
	}

	if (isResourceSetup)
	{
		mGraphicsDevice->UnBindUAVs(0, 1);
		mGraphicsDevice->UnbindGPUResources(SKINNING_SLOT_VERTEX_POS, 2);
	}

	mGraphicsDevice->EndEvent();
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