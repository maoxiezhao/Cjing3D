#include "renderer.h"
#include "shaderLib.h"
#include "preset\renderPreset.h"
#include "2D\renderer2D.h"
#include "textureHelper.h"
#include "renderer\RHI\device.h"
#include "core\globalContext.hpp"
#include "helper\profiler.h"
#include "renderer\RHI\d3d11\deviceD3D11.h"
#include "system\component\camera.h"
#include "resource\resourceManager.h"
#include "system\sceneSystem.h"
#include "pipelineStates\pipelineStateManager.h"

#ifdef _WIN32
#include "platform\win32\gameWindowWin32.h"
#endif

// render pass
#include "renderer\passes\terrain\terrainPass.h"
#include "renderer\passes\particle\particlePass.h"

namespace Cjing3D {
namespace Renderer {
namespace {

///////////////////////////////////////////////////////////////////////////////////
//  renderer members
///////////////////////////////////////////////////////////////////////////////////
	bool mIsInitialized = false;
	U32 mFrameNum = 0;
	U32x2 mScreenSize = U32x2();
	F32 mGamma = 2.2f;
	CommonCB mCommonCB = {};
	std::vector<int> mPendingUpdateMaterials;
	LinearAllocator mFrameAllocator;
	LinearAllocator mRenderAllocator[GraphicsDevice::MAX_COMMANDLIST_COUNT];

	std::shared_ptr<GraphicsDevice> mGraphicsDevice = nullptr;

	// base member
	std::unique_ptr<ShaderLib> mShaderLib = nullptr;
	std::unique_ptr<RenderPreset> mRenderPreset = nullptr;
	std::unique_ptr<DeferredMIPGenerator> mDeferredMIPGenerator = nullptr;
	std::unique_ptr<PipelineStateManager> mPipelineStateManager = nullptr;
	std::unique_ptr<RenderPath> mCurrentRenderPath = nullptr;

	// frame data
	std::unordered_map<const CameraComponent*, FrameCullings> mFrameCullings;
	RenderFrameData mFrameData;

	// render pass
	std::map<StringID, std::shared_ptr<RenderPass>> mRenderPassMap;

	// debug world
	I32x2 mDebugGridSize = I32x2(0, 0);
	I32x2 mSavedDebugGridSize = I32x2(0, 0);
	F32x3 mDebugGridPosOffset = F32x3(0.0f, 0.0f, 0.0f);
	I32 mDebugGridVertexCount = 0;
	GPUBuffer mDebugGridVertexBuffer;

	// connection
	ScopedConnection mResolutionChangedConn;

///////////////////////////////////////////////////////////////////////////////////
//  renderer variants
///////////////////////////////////////////////////////////////////////////////////
	// definitions, only use in this cpp.
	const U32 MAX_FRAME_ALLOCATOR_SIZE = 4 * 1024 * 1024;

///////////////////////////////////////////////////////////////////////////////////
//  shadow pass
///////////////////////////////////////////////////////////////////////////////////
	// 当前阴影级联数量
	U32 shadowCascadeCount = 3;
	F32 shadowCascadeCoefficient = 0.75;
	U32 shadowRes2DResolution = 1024;
	U32 shadowMap2DCount = 8;
	std::vector<F32> currentCascadeSplits = {
		0.0f,
		0.01f,
		0.05f,
		1.0f
	};

	Texture2D shadowMapArray2D;
	std::vector<RenderBehavior> shadowRenderBehaviors;
	std::vector<bool> shadowMapDirtyTable;

	// 级联子视锥体
	struct LightShadowCamera
	{
		Frustum mFrustum;
		XMMATRIX mViewProjection;
		DirectX::BoundingFrustum mBoundingFrustum;

		LightShadowCamera() : mViewProjection(XMMatrixIdentity()){}
		LightShadowCamera(const XMVECTOR& eyePos, const XMVECTOR& rot, F32 nearZ, F32 farZ, F32 fov)
		{
			const XMVECTOR q = XMQuaternionNormalize(rot);
			const XMMATRIX matRot = XMMatrixRotationQuaternion(q);
			// create light camera view matrix
			// light default dir is (0, -1, 0), up(0, 0, 1)
			const XMVECTOR to = XMVector3TransformNormal(XMVectorSet(0.0f,-1.0f, 0.0f, 0.0f), matRot);
			const XMVECTOR up = XMVector3TransformNormal(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), matRot);
			const XMMATRIX view = XMMatrixLookToLH(eyePos, to, up);
			const XMMATRIX proj = XMMatrixPerspectiveFovLH(fov, 1.0f, farZ, nearZ);	// reversed depth buffer!!!
			mViewProjection = view * proj;
			mFrustum.SetupFrustum(mViewProjection);

			// create bounding frustum and transform to world space
			mBoundingFrustum = CreateBoundingFrustum(proj);
			TransformBoundingFrustum(mBoundingFrustum, XMMatrixInverse(nullptr, view));
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

	void SetShadowMap2DProperty(U32 resolution, U32 count)
	{
		// 阴影贴图是一系列仅保存深度信息的贴图
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
		
		GraphicsDevice& device = GetDevice();
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

	void SetShadowMapCubeProperty(U32 resolution, U32 count)
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

		GraphicsDevice& device = GetDevice();
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
		// 创建视锥体级联分割
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
		csfs.clear();

		if (currentCascadeSplits.size() != (size_t)(cascadeCount) + 1) {
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
		const XMVECTOR up = XMVector3TransformNormal(XMVectorSet(0, 0,  1, 0), rotation);
		const XMVECTOR to = XMVector3TransformNormal(XMVectorSet(0, -1, 0, 0), rotation);
		const XMMATRIX lightView = XMMatrixLookToLH(XMVectorZero(), to, up);

		// 对每个层级创建对应的阴影相机
		// 将所有子视锥体转换到光照相机空间后，创建boundingBox和view projection
		for (int cascade = 0; cascade < cascadeCount; cascade++)
		{
			const F32 split_near = currentCascadeSplits[cascade];
			const F32 split_far  = currentCascadeSplits[cascade + 1];

			// 将子视锥体转换到light space
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

			XMVECTOR vRadius = XMVectorReplicate(radius);
			XMVECTOR vMin = center - vRadius;
			XMVECTOR vMax = center + vRadius;

			// 对齐到纹理网格上
			const XMVECTOR texelSize = XMVectorSubtract(vMax, vMin) / (F32)shadowRes2DResolution;
			vMin = XMVectorFloor(vMin / texelSize) * texelSize;
			vMax = XMVectorFloor(vMax / texelSize) * texelSize;
			center = (vMin + vMax) * 0.5f;
			
			// 子视锥体本身包围盒在z轴方向过小，拉伸包围盒以包含更多的物体（例如视锥体之外的物体，但在光源方向中也可能产生阴影）
			XMFLOAT3 _min, _max, _center;
			XMStoreFloat3(&_min, vMin);
			XMStoreFloat3(&_max, vMax);
			XMStoreFloat3(&_center, center);

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

/////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////
// Tiled light culling
U32 lastScreenWidth = 0;
U32 lastScreenHeight = 0;
XMMATRIX lastCameraViewMatrix;

bool bIsTiledCullingDebug = false;
Texture2D tiledCullingDebugTexture;
void UpdateTiledCullingDebugTexture(GraphicsDevice& device, U32 width, U32 height)
{
	if (!bIsTiledCullingDebug) {
		return;
	}

	if (!tiledCullingDebugTexture.IsValid())
	{
		TextureDesc desc = {};
		desc.mWidth = width;
		desc.mHeight = height;
		desc.mMipLevels = 1;
		desc.mFormat = FORMAT_R8G8B8A8_UNORM;
		desc.mBindFlags = BIND_UNORDERED_ACCESS | BIND_SHADER_RESOURCE;

		const auto result = device.CreateTexture2D(&desc, nullptr, tiledCullingDebugTexture);
		Debug::ThrowIfFailed(result, "Failed to create tiledCullingDebugTexture:%08x", result);
		device.SetResourceName(tiledCullingDebugTexture, "tiledCullingDebugTexture");
	}
}

bool IsTiledCullingDebug()
{
	return bIsTiledCullingDebug;
}

U32x2 GetCullingTiledCount()
{
	U32x2 screenSize = GetScreenSize();
	return {
		(screenSize[0] + LIGHT_CULLING_TILED_BLOCK_SIZE - 1) / LIGHT_CULLING_TILED_BLOCK_SIZE,
		(screenSize[1] + LIGHT_CULLING_TILED_BLOCK_SIZE - 1) / LIGHT_CULLING_TILED_BLOCK_SIZE
	};
}

void TiledLightCulling(CommandList cmd, Texture2D& depthBuffer)
{
	PROFILER_BEGIN_GPU_BLOCK(cmd, "TiledLightCulling");

	// 1. 在当前分辨率下创建用于保存frustum的structuredBuffer FrustumBuffer
	// frustum buffer 保存4个裁剪面F32x4 * 4
	bool isScreenSizeChanged = false;
	auto screenSize = GetScreenSize();
	if (lastScreenWidth != screenSize[0] || lastScreenHeight != screenSize[1])
	{
		isScreenSizeChanged = true;
		lastScreenWidth = screenSize[0];
		lastScreenHeight = screenSize[1];
	}

	U32x2 cullingTiledCount = GetCullingTiledCount();
	GPUBuffer& frustumBuffer = mRenderPreset->GetOrCreateCustomBuffer(("TiledFrustumBuffer"));
	if (isScreenSizeChanged || !frustumBuffer.IsValid())
	{
		GPUBufferDesc desc = {};
		desc.mUsage = USAGE_DEFAULT;
		desc.mCPUAccessFlags = 0;
		desc.mBindFlags = BIND_SHADER_RESOURCE | BIND_UNORDERED_ACCESS;
		desc.mMiscFlags = RESOURCE_MISC_BUFFER_STRUCTURED;
		desc.mStructureByteStride = sizeof(XMFLOAT4) * 4;
		desc.mByteWidth = desc.mStructureByteStride * cullingTiledCount[0] * cullingTiledCount[1];

		const auto result = mGraphicsDevice->CreateBuffer(&desc, frustumBuffer, nullptr);
		Debug::ThrowIfFailed(result, "failed to create tiled frustum buffer:%08x", result);
		mGraphicsDevice->SetResourceName(frustumBuffer, "TiledFrustumBuffer");
	}

	// 2. 如果分辨率发生改变，意味着tileCount发生改变，需要重新构建每个tile的light列表
	GPUBuffer& tiledLightBuffer = mRenderPreset->GetOrCreateCustomBuffer("TiledLightBuffer");
	if (isScreenSizeChanged || !tiledLightBuffer.IsValid())
	{
		GPUBufferDesc desc = {};
		desc.mUsage = USAGE_DEFAULT;
		desc.mCPUAccessFlags = 0;
		desc.mBindFlags = BIND_SHADER_RESOURCE | BIND_UNORDERED_ACCESS;
		desc.mMiscFlags = RESOURCE_MISC_BUFFER_STRUCTURED;
		desc.mStructureByteStride = sizeof(U32);
		desc.mByteWidth = desc.mStructureByteStride * cullingTiledCount[0] * cullingTiledCount[1] * SHADER_LIGHT_TILE_BUCKET_COUNT;

		const auto result = mGraphicsDevice->CreateBuffer(&desc, tiledLightBuffer, nullptr);
		Debug::ThrowIfFailed(result, "failed to create tiled light buffer:%08x", result);
		mGraphicsDevice->SetResourceName(tiledLightBuffer, "TiledLightBuffer");
	}

	if (IsTiledCullingDebug() && isScreenSizeChanged) {
		UpdateTiledCullingDebugTexture(*mGraphicsDevice, screenSize[0], screenSize[1]);
	}

	BindCommonResource(cmd);

	// 3. 构建每一个tile的frustum，保存在FrustumBuffer。仅当当前相机的view矩阵发生变化时执行
	CameraComponent& camera = GetCamera();
	XMMATRIX currentViewMatrix = camera.GetViewMatrix();
	if (isScreenSizeChanged || XMMatrixCompare(currentViewMatrix, lastCameraViewMatrix) == false)
	{
		lastCameraViewMatrix = currentViewMatrix;

		mGraphicsDevice->BeginEvent(cmd, "BuildTiledFrustum");
		mGraphicsDevice->BindComputeShader(cmd, mRenderPreset->GetComputeShader(ComputeShaderType_TiledFrustum));

		CSParamsCB cb;
		cb.gCSNumThreads.x = cullingTiledCount[0];
		cb.gCSNumThreads.y = cullingTiledCount[1];

		GPUBuffer& buffer = mRenderPreset->GetConstantBuffer(ConstantBufferType_CSParams);
		mGraphicsDevice->UpdateBuffer(cmd, buffer, &cb, sizeof(CSParamsCB));
		mGraphicsDevice->BindConstantBuffer(cmd, SHADERSTAGES_CS, buffer, CB_GETSLOT_NAME(CSParamsCB));

		GPUResource* uavs[] = { &frustumBuffer };
		mGraphicsDevice->BindUAVs(cmd, uavs, 0, 1);
		mGraphicsDevice->Dispatch(cmd, 
			(cullingTiledCount[0] + LIGHT_CULLING_TILED_BLOCK_SIZE - 1) / LIGHT_CULLING_TILED_BLOCK_SIZE,
			(cullingTiledCount[1] + LIGHT_CULLING_TILED_BLOCK_SIZE - 1) / LIGHT_CULLING_TILED_BLOCK_SIZE,
			1
		);
		mGraphicsDevice->UnBindUAVs(cmd, 0, 1);
		mGraphicsDevice->EndEvent(cmd);
	}

	// 4. 对每一个tile执行光照裁剪，并获得一个有效Entity的bucket
	{
		mGraphicsDevice->BeginEvent(cmd, "LightTiledCulling");
		mGraphicsDevice->BindComputeShader(cmd, mRenderPreset->GetComputeShader(ComputeShaderType_LightTiledCulling));

		// bind resources
		mGraphicsDevice->UnbindGPUResources(cmd, SBSLOT_TILED_LIGHTS, 1);
		mGraphicsDevice->BindGPUResource(cmd, SHADERSTAGES_CS, &depthBuffer, TEXTURE_SLOT_DEPTH);
		mGraphicsDevice->BindGPUResource(cmd, SHADERSTAGES_CS, &frustumBuffer, SBSLOT_TILED_FRUSTUMS);

		FrameCullings& frameCulling = mFrameCullings.at(&camera);
		CSParamsCB cb;
		cb.gCSNumThreads.x = cullingTiledCount[0] * LIGHT_CULLING_TILED_BLOCK_SIZE;
		cb.gCSNumThreads.y = cullingTiledCount[1] * LIGHT_CULLING_TILED_BLOCK_SIZE;
		cb.gCSNumThreadGroups.x = cullingTiledCount[0];
		cb.gCSNumThreadGroups.y = cullingTiledCount[1];
		cb.gCSNumLights = frameCulling.mCulledLights.size();

		GPUBuffer& buffer = mRenderPreset->GetConstantBuffer(ConstantBufferType_CSParams);
		mGraphicsDevice->UpdateBuffer(cmd, buffer, &cb, sizeof(CSParamsCB));
		mGraphicsDevice->BindConstantBuffer(cmd, SHADERSTAGES_CS, buffer, CB_GETSLOT_NAME(CSParamsCB));

		// bind output texture
		GPUResource* uavs[] = { &tiledLightBuffer };
		mGraphicsDevice->BindUAVs(cmd, uavs, 0, 1);

		if (IsTiledCullingDebug()) {
			mGraphicsDevice->BindUAV(cmd, &tiledCullingDebugTexture, 1);
		}

		mGraphicsDevice->Dispatch(
			cmd,
			cb.gCSNumThreadGroups.x,
			cb.gCSNumThreadGroups.y,
			1
		);
		mGraphicsDevice->UnBindAllUAVs(cmd);
		mGraphicsDevice->EndEvent(cmd);
	}

	PROFILER_END_BLOCK();
}
/////////////////////////////////////////////////////////////////////////////////////////////////////

void RenderFrameData::Clear()
{
	mShaderLightArrayCount = 0;
}

void Initialize()
{
	if (mIsInitialized == true){
		return;
	}

	if (mGraphicsDevice == nullptr) {
		Debug::Die("Renderer::Initialize must set graphics device.");
	}

	mScreenSize = mGraphicsDevice->GetScreenSize();

	// initialize camera
	GetCamera().SetupPerspective((F32)mScreenSize[0], (F32)mScreenSize[1], 0.1f, 800.0f);
	GetCamera().SetCameraStatus(
		{ 0.0f, 0.0f, -50.0f },
		{ 0.0f, 0.0f,   1.0f },
		{ 0.0f, 1.0f,   0.0f }
	);

	// initialize states
	mRenderPreset = std::make_unique<RenderPreset>(*mGraphicsDevice);
	mRenderPreset->Initialize();

	// initialize shader
	mShaderLib = std::make_unique<ShaderLib>();
	mShaderLib->Initialize();

	// initialize mip generator
	mDeferredMIPGenerator = std::make_unique<DeferredMIPGenerator>();

	// initialize 2d renderer
	Renderer2D::Initialize();

	// initialize texture helper
	TextureHelper::Initialize();

	// initialize PipelineStates
	mPipelineStateManager = std::make_unique<PipelineStateManager>();
	mPipelineStateManager->Initialize();

	// initialize frame cullings
	mFrameCullings[&GetCamera()] = FrameCullings();

	mFrameData.mFrameScreenSize = { (F32)mScreenSize[0], (F32)mScreenSize[1] };
	mFrameData.mFrameInvScreenSize = { 1.0f / (F32)mScreenSize[0], 1.0f / (F32)mScreenSize[1] };
	mFrameData.mFrameAmbient = { 0.1f, 0.1f, 0.1f };

	SetShadowMap2DProperty(shadowRes2DResolution, shadowMap2DCount);
	SetShadowMapCubeProperty(shadowResCubeResolution, shadowMapCubeCount);

	// initialize render pass
	InitializeRenderPasses();

	// event callbacks
	mResolutionChangedConn = EventSystem::Register(EVENT_RESOLUTION_CHANGE,
		[](const VariantArray& variants) {
			const U32 width  = variants[0].GetValue<U32>();
			const U32 height = variants[1].GetValue<U32>();

			SetScreenSize(width, height);
		});

	mIsInitialized = true;
}

void Uninitialize()
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
	Renderer2D::Uninitialize();

	mPipelineStateManager->Uninitialize();
	mRenderPreset->Uninitialize();
	mShaderLib->Uninitialize();

	mFrameAllocator.FreeBuffer();
	for (int i = 0; i < GraphicsDevice::MAX_COMMANDLIST_COUNT; i++) {
		mRenderAllocator[i].FreeBuffer();
	}

	mPipelineStateManager.reset();
	mRenderPreset.reset();
	mShaderLib.reset();

	UninitializeDevice();

	mIsInitialized = false;
}

void UninitializeDevice()
{
	if (mGraphicsDevice != nullptr)
	{
		mGraphicsDevice.reset();
		mGraphicsDevice = nullptr;
	}
}

bool IsInitialized()
{
	return mIsInitialized;
}

void FixedUpdate()
{
}

void Update(F32 deltaTime)
{
	mFrameData.mFrameDeltaTime = deltaTime;
	mFrameData.mFrameTime += deltaTime;

	if (mCurrentRenderPath != nullptr) {
		mCurrentRenderPath->Update(deltaTime);
	}
}

// 在update中执行
void UpdatePerFrameData(F32 deltaTime, U32 renderLayerMask)
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

		// 只处理当前主相机的culling
		const CameraComponent* camera = kvp.first;
		if (camera == nullptr && camera != &mainCamera) {
			continue;
		}

		auto currentFrustum = camera->GetFrustum();
		frameCulling.mFrustum = currentFrustum;

		// 遍历场景所有物体的aabb,如果在相机范围内，则添加物体的index
		auto& objectAABBs = scene.mObjectAABBs;
		for (size_t i = 0; i < objectAABBs.GetCount(); i++)
		{
			auto aabb = objectAABBs[i];
			if (aabb == nullptr) {
				continue;
			}

			auto layer = scene.mLayers.GetComponent(aabb->GetCurrentEntity());
			if (layer != nullptr && !(layer->GetLayerMask() & renderLayerMask)) {
				continue;
			}

			if (currentFrustum.Overlaps(aabb->GetAABB())) {
				frameCulling.mCulledObjects.push_back((U32)i);
			}
		}

		// 遍历场景的light aabbs
		auto& lightAABBs = scene.mLightAABBs;
		for (size_t i = 0; i < lightAABBs.GetCount(); i++)
		{
			auto aabb = lightAABBs[i];
			if (aabb == nullptr) {
				continue;
			}

			auto layer = scene.mLayers.GetComponent(aabb->GetCurrentEntity());
			if (layer != nullptr && !(layer->GetLayerMask() & renderLayerMask)) {
				continue;
			}

			if (currentFrustum.Overlaps(aabb->GetAABB()))
			{
				frameCulling.mCulledLights.push_back((U32)i);
			}
		}

		// 遍历场景的particle
		auto& particles = scene.mParticles;
		for (size_t i = 0; i < particles.GetCount(); i++)
		{
			auto particle = particles[i];
			if (particle == nullptr) {
				continue;
			}

			auto layer = scene.mLayers.GetComponent(particle->GetCurrentEntity());
			if (layer != nullptr && !(layer->GetLayerMask() & renderLayerMask)) {
				continue;
			}

			frameCulling.mCulledParticles.push_back((U32)i);
		}

		LinearAllocator& frameAllocator = GetFrameAllocator();

		F32x3 cameraPos = camera->GetCameraPos();
		U32 culledLightCount = frameCulling.mCulledLights.size();
		if (culledLightCount > 0)
		{
			auto& culledLights = frameCulling.mCulledLights;

			// 对于裁剪到的光源，根据光源由近到远分配shadowMap
			// 创建一个数字数组，每个数字同时保存光源的距离和索引（前后16位），一种特殊的排序技巧
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
				U32 lightIndex = lightSortingHashes[i] & 0xffff;
				auto light = scene.mLights[lightIndex];
				if (light == nullptr) {
					continue;
				}

				// 默认无阴影贴图
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

// 在render阶段render前执行
void RefreshRenderData(CommandList cmd)
{
	mFrameData.Clear();

	BindCommonResource(cmd);

	auto& mainScene = GetMainScene();

	// 处理延时生成的mipmap
	if (mDeferredMIPGenerator != nullptr) {
		mDeferredMIPGenerator->UpdateMipGenerating(cmd);
	}

	CameraComponent& mainCamera = GetCamera();

	// 处理更新material data 
	for (int materialIndex : mPendingUpdateMaterials)
	{
		auto material = mainScene.mMaterials[materialIndex];

		ShaderMaterial sm = material->CreateMaterialCB();
		mGraphicsDevice->UpdateBuffer(cmd, material->GetConstantBuffer(), &sm, sizeof(sm));
	}

	LinearAllocator& renderAllocator = GetRenderAllocator(cmd);

	// 更新shaderLightArray buffer
	ShaderLight* shaderLights = (ShaderLight*)renderAllocator.Allocate(sizeof(ShaderLight) * SHADER_LIGHT_COUNT);
	U32 lightCount = 0;

	// 更新shaderMatrixArray buffer
	XMMATRIX* shaderMatrixs = (XMMATRIX*)renderAllocator.Allocate(sizeof(XMMATRIX) * SHADER_MATRIX_COUNT);
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

		// 如果light支持阴影，且已分配了shadowMapIndex，则传递对应
		// shader viewProjection Matrix和shaderMapIndex
		if (light->IsCastShadow() && light->GetShadowMapIndex() >= 0)
		{
			shaderLight.shadowKernel = (1.0f / shadowRes2DResolution);
			shaderLight.SetShadowData(currentMatrixIndex, light->GetShadowMapIndex());
			switch (light->GetLightType())
			{
			case Cjing3D::LightComponent::LightType_Directional:
			{
				// TODO:在renderShadow部分又计算了一次。。
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
	mGraphicsDevice->UpdateBuffer(cmd, mRenderPreset->GetStructuredBuffer(StructuredBufferType_ShaderLight), shaderLights, sizeof(ShaderLight) * lightCount);
	mGraphicsDevice->UpdateBuffer(cmd, mRenderPreset->GetStructuredBuffer(StructuredBufferType_MatrixArray), shaderMatrixs, sizeof(XMMATRIX) * currentMatrixIndex);

	renderAllocator.Free(sizeof(XMMATRIX) * SHADER_MATRIX_COUNT);
	renderAllocator.Free(sizeof(ShaderLight) * SHADER_LIGHT_COUNT);

	// 更新每一帧的基本信息
	UpdateFrameCB(cmd);

	// mesh skinning
	ProcessSkinning(cmd);

	// particle
	ProcessParticles(cmd, mainScene, frameCulling.mCulledParticles);

	// refresh render pass 
	for (auto kvp : mRenderPassMap)
	{
		auto renderPass = kvp.second;
		if (renderPass != nullptr) {
			renderPass->RefreshRenderData(cmd);
		}
	}
}

void EndFrame()
{
	mFrameAllocator.Reset();
	for (int i = 0; i < GraphicsDevice::MAX_COMMANDLIST_COUNT; i++) {
		mRenderAllocator[i].Reset();
	}
}

void SetScreenSize(U32 width, U32 height)
{
	if (width != mScreenSize[0] || height != mScreenSize[1])
	{
		mScreenSize[0] = width;
		mScreenSize[1] = height;

		mFrameData.mFrameScreenSize = { (F32)mScreenSize[0], (F32)mScreenSize[1] };
		mFrameData.mFrameInvScreenSize = { 1.0f / (F32)mScreenSize[0], 1.0f / (F32)mScreenSize[1] };

		GetCamera().SetupPerspective((F32)width, (F32)height, 0.1f, 800.0f);
		GetDevice().SetResolution({ width, height });
	}
}

U32x2 GetScreenSize()
{
	return mScreenSize;
}

XMMATRIX GetScreenProjection()
{
	return XMMatrixOrthographicOffCenterLH(0, (F32)mScreenSize[0], (F32)mScreenSize[1], 0, -1, 1);
}

Ray GetMainCameraMouseRay(const U32x2& pos)
{
	return GetCamera().ScreenPointToRay(pos);
}

GraphicsDevice & GetDevice()
{
	return *mGraphicsDevice;
}

CameraComponent& GetCamera()
{
	static CameraComponent mCamera;
	return mCamera;
}

ShaderLib & GetShaderLib()
{
	return *mShaderLib;
}

RenderPreset & GetRenderPreset()
{
	return *mRenderPreset;
}

void SetDevice(const std::shared_ptr<GraphicsDevice>& device)
{
	mGraphicsDevice = device;
}

void SetGamma(F32 gamma)
{
	mGamma = gamma;
}

F32 GetGamma()
{
	return mGamma;
}

U32 GetShadowCascadeCount()
{
	return shadowCascadeCount;
}

U32 GetShadowRes2DResolution()
{
	return shadowRes2DResolution;
}

F32x3 GetAmbientColor()
{
	return mFrameData.mFrameAmbient;
}

void SetAmbientColor(F32x3 color)
{
	mFrameData.mFrameAmbient = color;
}

void SetAlphaCutRef(CommandList cmd, F32 alpha)
{
	F32 newAlphaRef = 1.0f - alpha + 1.0f / 256.0f; // 加上一个单位(1.0f/256.f)使结果偏大
	if (newAlphaRef != mCommonCB.gCommonAlphaCutRef)
	{
		mCommonCB.gCommonAlphaCutRef = newAlphaRef;
		mGraphicsDevice->UpdateBuffer(cmd, mRenderPreset->GetConstantBuffer(ConstantBufferType_Common), &mCommonCB, sizeof(CommonCB));
	}
}

void ResetAlphaCutRef(CommandList cmd)
{
	SetAlphaCutRef(cmd, 1.0f);
}

void RenderShadowmaps(CommandList cmd, CameraComponent& camera, const U32 renderLayerMask)
{
	FrameCullings& frameCulling = mFrameCullings[&camera];
	auto& culledLights = frameCulling.mCulledLights;
	if (culledLights.empty()) {
		return;
	}
	auto& scene = GetMainScene();

	PROFILER_BEGIN_GPU_BLOCK(cmd, "RenderShadowmaps");
	mGraphicsDevice->BeginEvent(cmd, "RenderShadowmaps");

	BindCommonResource(cmd);

	// 为了写入shadowmap,需要先Unbind GPUResource
	mGraphicsDevice->UnbindGPUResources(cmd, TEXTURE_SLOT_SHADOW_ARRAY_2D, 1);

	// 根据光源类型依次创建cascade shadowmaps
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
				RenderDirLightShadowmap(cmd, *light, camera, renderLayerMask);
				break;
			case Cjing3D::LightComponent::LightType_Point:
				RenderPointLightShadowmap(cmd, *light, camera, renderLayerMask);
				break;
			default:
				break;
			}
		}
	}

	mGraphicsDevice->BindRenderTarget(cmd, 0, nullptr, nullptr);
	mGraphicsDevice->EndEvent(cmd);
	PROFILER_END_BLOCK();
}

void RenderSceneOpaque(CommandList cmd, CameraComponent& camera, RenderPassType renderPassType)
{
	mGraphicsDevice->BeginEvent(cmd, "RenderScene");

	BindCommonResource(cmd);
	BindShadowMaps(cmd, SHADERSTAGES_PS);

	if (renderPassType == RenderPassType_TiledForward) 
	{
		GPUBuffer& tiledLightBuffer = mRenderPreset->GetOrCreateCustomBuffer("TiledLightBuffer");
		if (tiledLightBuffer.IsValid()) {
			mGraphicsDevice->BindGPUResource(cmd, SHADERSTAGES_PS, &tiledLightBuffer, SBSLOT_TILED_LIGHTS);
		}
	}

	// terrain render
	GetRenderPass(STRING_ID(TerrainPass))->Render(cmd);
	// impostor render
	RenderImpostor(cmd, camera, renderPassType);

	LinearAllocator& renderAllocator = GetRenderAllocator(cmd);
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
			object->IsRenderable() == false ||
			object->GetRenderableType() != RenderableType_Opaque) {
			continue;
		}

		// 默认当object设置为Impostor，仅仅在RenderImpostor时渲染
		// 未来还是会设置一个distance，超过distance时才仅仅渲染Impostor
		if (object->IsImpostor()) {
			continue;
		}

		RenderBatch* renderBatch = (RenderBatch*)renderAllocator.Allocate(sizeof(RenderBatch));
		renderBatch->Init(objectEntity, object->mMeshID, DistanceEstimated(cameraPos, object->mCenter));

		renderQueue.AddBatch(renderBatch);
	}

	if (renderQueue.IsEmpty() == false)
	{
		renderQueue.Sort(RenderQueue::FrontToBack);
		ProcessRenderQueue(cmd, renderQueue, renderPassType, RenderableType_Opaque);

		renderAllocator.Free(renderQueue.GetBatchCount() * sizeof(RenderBatch));
	}

	mGraphicsDevice->EndEvent(cmd);
}

void RenderSceneTransparent(CommandList cmd, CameraComponent& camera, RenderPassType renderPassType)
{
	mGraphicsDevice->BeginEvent(cmd, "RenderSceneTransparent");

	BindShadowMaps(cmd, SHADERSTAGES_PS);

	LinearAllocator& frameAllocator = GetRenderAllocator(cmd);
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
		// 渲染透明物体时需要从远处往近处渲染
		renderQueue.Sort(RenderQueue::BackToFront);
		ProcessRenderQueue(cmd, renderQueue, renderPassType, RenderableType_Transparent);

		frameAllocator.Free(renderQueue.GetBatchCount() * sizeof(RenderBatch));
	}

	mGraphicsDevice->EndEvent(cmd);
}

void RenderImpostor(CommandList cmd, CameraComponent& camera, RenderPassType renderPassType)
{
	mGraphicsDevice->BeginEvent(cmd, "RenderImpostor");

	mGraphicsDevice->EndEvent(cmd);
}

void RenderSky(CommandList cmd)
{
	mGraphicsDevice->BeginEvent(cmd, "RenderSky");

	auto& scene = GetMainScene();
	if (scene.mWeathers.GetCount() > 0)
	{
		auto currentWeather = scene.mWeathers.GetComponentByIndex(0);
		if (currentWeather->mSkyMap != nullptr)
		{
			PipelineState* pso = mPipelineStateManager->GetPipelineStateByID(PipelineStateID_SkyRendering);
			if (pso != nullptr && pso->IsValid() && currentWeather->mSkyMap != nullptr)
			{
				mGraphicsDevice->BindPipelineState(cmd, pso);
				mGraphicsDevice->BindGPUResource(
					cmd,
					SHADERSTAGES_PS, 
					currentWeather->mSkyMap->mTexture,
					TEXTURE_SLOT_GLOBAL_ENV_MAP
				);
			}
		}

		mGraphicsDevice->Draw(cmd, 3, 0);
	}

	mGraphicsDevice->EndEvent(cmd);
}

void RenderLinearDepth(CommandList cmd, Texture2D& depthBuffer, Texture2D& linearDepthBuffer)
{
	mGraphicsDevice->BeginEvent(cmd, "RenderLinearDepth");

	// bind post process buffer
	const TextureDesc linearDepthDesc = linearDepthBuffer.GetDesc();
	PostprocessCB cb;
	cb.gPPResolution.x = (U32)(linearDepthDesc.mWidth * 0.5f);
	cb.gPPResolution.y = (U32)(linearDepthDesc.mHeight * 0.5f);
	cb.gPPInverseResolution.x = (1.0f / cb.gPPResolution.x);
	cb.gPPInverseResolution.y = (1.0f / cb.gPPResolution.y);

	const TextureDesc& depthDesc = depthBuffer.GetDesc();
	cb.gPPParam1 = (F32)depthDesc.mWidth;
	cb.gPPParam2 = (F32)depthDesc.mHeight;
	cb.gPPParam3 = (1.0f / cb.gPPParam1);
	cb.gPPParam4 = (1.0f / cb.gPPParam2);
	GPUBuffer& postprocessBuffer = mRenderPreset->GetConstantBuffer(ConstantBufferType_Postprocess);
	mGraphicsDevice->UpdateBuffer(cmd, postprocessBuffer, &cb, sizeof(PostprocessCB));
	mGraphicsDevice->BindConstantBuffer(cmd, SHADERSTAGES_CS, postprocessBuffer, CB_GETSLOT_NAME(PostprocessCB));

	// bind shader
	mGraphicsDevice->BindGPUResource(cmd, SHADERSTAGES_CS, &depthBuffer, TEXTURE_SLOT_UNIQUE_0);
	mGraphicsDevice->BindComputeShader(cmd, mRenderPreset->GetComputeShader(ComputeShaderType_LinearDepth));

	// bind output texture
	for (int mipmap = 0; mipmap < 6; mipmap++) {
		mGraphicsDevice->BindUAV(cmd, &linearDepthBuffer, mipmap, mipmap);
	}
	mGraphicsDevice->Dispatch(
		cmd, 
		(U32)((linearDepthDesc.mWidth  + SHADER_LINEARDEPTH_BLOCKSIZE - 1) / SHADER_LINEARDEPTH_BLOCKSIZE),
		(U32)((linearDepthDesc.mHeight + SHADER_LINEARDEPTH_BLOCKSIZE - 1) / SHADER_LINEARDEPTH_BLOCKSIZE),
		1
	);
	mGraphicsDevice->UnBindUAVs(cmd, 0, 6);
	mGraphicsDevice->EndEvent(cmd);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// RenderSSAO
U32x2 savedAOTextureSize = U32x2(0u, 0u);
Texture2D aoTemp;
Texture2D aoTemp1;
Texture2D aoDebug;
bool ssaoDebug = false;
void RenderSSAO(CommandList cmd, Texture2D& depthBuffer, Texture2D& linearDepthBuffer, Texture2D& aoTexture, F32 aoRange, U32 aoSampleCount)
{
	// SSAO
	// 1. 基于linnearDepthBuffer，根据AORange计算屏幕空间（ScreenSize)每个像素的遮蔽量，写到AOTemp中
	// 2. 对得到的纹理做一次模糊，目的是为了缓解低采样量下的Banding问题
	// 3. 因为临时的AOTemp大小问题（减轻Blur的消耗）,再执行Bilateral Unsample得到原始大小的AOTexture

	PROFILER_BEGIN_GPU_BLOCK(cmd, "SSAO");
	mGraphicsDevice->BeginEvent(cmd, "SSAO");

	// 0.创建temp texture,这里只创建四分之一大小的纹理，最后会unsample得到原始大小
	auto& linearDesc = linearDepthBuffer.GetDesc();
	if (savedAOTextureSize.x() != linearDesc.mWidth || savedAOTextureSize.y() != linearDesc.mHeight)
	{
		savedAOTextureSize[0] = linearDesc.mWidth;
		savedAOTextureSize[1] = linearDesc.mHeight;

		TextureDesc desc = {};
		desc.mWidth = linearDesc.mWidth / 2;
		desc.mHeight = linearDesc.mHeight / 2;
		desc.mFormat = FORMAT_R8_UNORM;
		desc.mBindFlags = BIND_SHADER_RESOURCE | BIND_UNORDERED_ACCESS;

		mGraphicsDevice->CreateTexture2D(&desc, nullptr, aoTemp);
		mGraphicsDevice->SetResourceName(aoTemp, "aoTextureTemp");
		mGraphicsDevice->CreateTexture2D(&desc, nullptr, aoTemp1);
		
		if (ssaoDebug)
		{
			TextureDesc descDebug = {};
			descDebug.mWidth = linearDesc.mWidth / 2;
			descDebug.mHeight = linearDesc.mHeight / 2;
			descDebug.mFormat = FORMAT_R32G32B32A32_FLOAT;
			descDebug.mBindFlags = BIND_SHADER_RESOURCE | BIND_UNORDERED_ACCESS;

			mGraphicsDevice->CreateTexture2D(&descDebug, nullptr, aoDebug);
			mGraphicsDevice->SetResourceName(aoDebug, "aoTextureDebug");
		}

	}

	// 1.计算AO
	auto& desc = aoTemp.GetDesc();
	PostprocessCB cb;
	cb.gPPResolution.x = desc.mWidth;
	cb.gPPResolution.y = desc.mHeight;
	cb.gPPInverseResolution.x = (1.0f / cb.gPPResolution.x);
	cb.gPPInverseResolution.y = (1.0f / cb.gPPResolution.y);
	cb.gPPParam1 = aoRange;
	cb.gPPParam2 = (F32)aoSampleCount;

	GPUBuffer& postprocessBuffer = mRenderPreset->GetConstantBuffer(ConstantBufferType_Postprocess);
	mGraphicsDevice->UpdateBuffer(cmd, postprocessBuffer, &cb, sizeof(PostprocessCB));
	mGraphicsDevice->BindConstantBuffer(cmd, SHADERSTAGES_CS, postprocessBuffer, CB_GETSLOT_NAME(PostprocessCB));

	// bind shader
	mGraphicsDevice->BindGPUResource(cmd, SHADERSTAGES_CS, &depthBuffer, TEXTURE_SLOT_DEPTH);
	mGraphicsDevice->BindGPUResource(cmd, SHADERSTAGES_CS, &linearDepthBuffer, TEXTURE_SLOT_LINEAR_DEPTH);
	mGraphicsDevice->BindComputeShader(cmd, mRenderPreset->GetComputeShader(ComputeShaderType_SSAO));

	// bind output texture
	if (ssaoDebug)
	{
		mGraphicsDevice->BindUAV(cmd, &aoTemp, 0);
		mGraphicsDevice->BindUAV(cmd, &aoDebug, 1);
		mGraphicsDevice->Dispatch(
			cmd, 
			(U32)((desc.mWidth + SHADER_POSTPROCESS_BLOCKSIZE - 1) / SHADER_POSTPROCESS_BLOCKSIZE),
			(U32)((desc.mHeight + SHADER_POSTPROCESS_BLOCKSIZE - 1) / SHADER_POSTPROCESS_BLOCKSIZE),
			1
		);
		mGraphicsDevice->UnBindUAVs(cmd, 0, 2);
	}
	else
	{
		mGraphicsDevice->BindUAV(cmd, &aoTemp, 0);
		mGraphicsDevice->Dispatch(
			cmd, 
			(U32)((desc.mWidth + SHADER_POSTPROCESS_BLOCKSIZE - 1) / SHADER_POSTPROCESS_BLOCKSIZE),
			(U32)((desc.mHeight + SHADER_POSTPROCESS_BLOCKSIZE - 1) / SHADER_POSTPROCESS_BLOCKSIZE),
			1
		);
		mGraphicsDevice->UnBindUAVs(cmd, 0, 1);
	}

	// 2.blur
	BilateralBlur(cmd, aoTemp, aoTemp1, aoTemp, linearDepthBuffer, 1.0f);

	// 3.Unsample
	UpsampleBilateral(cmd, aoTemp, aoTexture, linearDepthBuffer, 1.0f);

	mGraphicsDevice->EndEvent(cmd);
	PROFILER_END_CPU_GPU_BLOCK();
}

void RenderParticles(CommandList cmd, CameraComponent& camera, Texture2D& linearDepthBuffer)
{
	auto& culledParticles = mFrameCullings.at(&camera).mCulledParticles;
	if (culledParticles.empty()) {
		return;
	}

	Scene& scene = GetMainScene();
	LinearAllocator& renderAllocator = GetRenderAllocator(cmd);

	size_t culledParticleCount = culledParticles.size();
	F32x3 cameraPos = camera.GetCameraPos();
	U32* sortingHashes = (U32*)renderAllocator.Allocate(culledParticleCount * sizeof(U32));
	U32 hashIndex = 0;
	for (U32 i = 0; i < culledParticleCount; i++)
	{
		auto particle = scene.mParticles[culledParticles[i]];
		if (particle == nullptr) {
			continue;
		}

		F32 distance = DistanceEstimated(cameraPos, particle->mPosition);
		sortingHashes[hashIndex] = i & 0xffff;
		sortingHashes[hashIndex] |= (U32(distance * 10) & 0xffff) << 16;

		hashIndex++;
	}
	std::sort(sortingHashes, sortingHashes + hashIndex, std::greater<U32>());

	mGraphicsDevice->BindGPUResource(cmd, SHADERSTAGES_CS, &linearDepthBuffer, TEXTURE_SLOT_LINEAR_DEPTH);

	ParticlePass& particlePass = dynamic_cast<ParticlePass&>(*GetRenderPass(STRING_ID(ParticlePass)));
	for (U32 i = 0; i < hashIndex; i++)
	{
		auto particle = scene.mParticles[sortingHashes[i] & 0xffff];
		if (particle == nullptr) {
			continue;
		}

		MaterialComponent* material = scene.mMaterials.GetComponent(particle->GetCurrentEntity());
		particlePass.DrawParticle(cmd, *particle, *material);
	}

	renderAllocator.Free(culledParticleCount * sizeof(U32));
}

void RenderDebugScene(CommandList cmd, CameraComponent& camera)
{
	mGraphicsDevice->BeginEvent(cmd, "RenderDebugScene");
	// grid
	if (mDebugGridSize.x() > 0 && mDebugGridSize.y() > 0)
	{
		PipelineState* pso = mPipelineStateManager->GetPipelineStateByID(PipelineStateID_GridHelper);
		if (pso != nullptr && pso->IsValid() )
		{
			mGraphicsDevice->BindPipelineState(cmd, pso);
			if (!mDebugGridVertexBuffer.IsValid() || mSavedDebugGridSize != mDebugGridSize)
			{
				mSavedDebugGridSize = mDebugGridSize;

				const F32 h = 0.01f;
				const F32 width = 1.0f;
				const F32 color = 0.7f;
				const F32 halfWidth = mDebugGridSize.x() * width * 0.5f;
				const F32 halfHeight = mDebugGridSize.y() * width * 0.5f;
				std::vector<XMFLOAT4> verts;
				verts.resize(((mDebugGridSize.x() + 1) * 2 + (mDebugGridSize.y() + 1) * 2) * 2);

				size_t index = 0;
				// x axis
				for (int x = 0; x <= mDebugGridSize.x(); x++)
				{
					F32 currentX = x * width;
					verts[index++] = XMFLOAT4(currentX - halfWidth, h, -halfHeight, 1); // pos
					verts[index++] = XMFLOAT4(color, color, color, color);					      // color

					verts[index++] = XMFLOAT4(currentX - halfWidth, h, +halfHeight, 1); // pos
					verts[index++] = XMFLOAT4(color, color, color, color);						  // color
				}

				// x axis
				for (int y = 0; y <= mDebugGridSize.y(); y++)
				{
					F32 currentY = y * width;
					verts[index++] = XMFLOAT4(-halfWidth, h, currentY - halfHeight, 1); // pos
					verts[index++] = XMFLOAT4(color, color, color, color);						  // color

					verts[index++] = XMFLOAT4(+halfWidth, h, currentY - halfHeight, 1); // pos
					verts[index++] = XMFLOAT4(color, color, color, color);						  // color
				}
				mDebugGridVertexCount = verts.size() / 2;

				GPUBufferDesc desc = {};
				desc.mUsage = USAGE_IMMUTABLE;
				desc.mBindFlags = BIND_VERTEX_BUFFER;
				desc.mByteWidth = verts.size() * sizeof(XMFLOAT4);
				SubresourceData initData;
				initData.mSysMem = verts.data();

				const auto result = mGraphicsDevice->CreateBuffer(&desc, mDebugGridVertexBuffer, &initData);
				Debug::ThrowIfFailed(result, "failed to create debug grid vertex buffer:%08x", result);
			}

			RenderMiscCB cb;
			cb.gMiscColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			XMMATRIX offsetMat = XMMatrixTranslation(mDebugGridPosOffset.x(), mDebugGridPosOffset.y(), mDebugGridPosOffset.z());
			XMStoreFloat4x4(&cb.gMiscTransform, offsetMat * GetCamera().GetViewProjectionMatrix());
			
			GPUBuffer& buffer = mRenderPreset->GetConstantBuffer(ConstantBufferType_Misc);
			mGraphicsDevice->UpdateBuffer(cmd, buffer, &cb);
			mGraphicsDevice->BindConstantBuffer(cmd, SHADERSTAGES_VS, buffer, CB_GETSLOT_NAME(RenderMiscCB));
	
			GPUBuffer* vbs[] = {
				&mDebugGridVertexBuffer
			};
			const U32 strides[] = {
				sizeof(XMFLOAT4) + sizeof(XMFLOAT4)
			};
			mGraphicsDevice->BindVertexBuffer(cmd, vbs, 0, ARRAYSIZE(vbs), strides, nullptr);
			mGraphicsDevice->Draw(cmd, mDebugGridVertexCount, 0);
		}
	}
	else
	{
		mSavedDebugGridSize = mDebugGridSize;
	}

	mGraphicsDevice->EndEvent(cmd);
}

void SetDebugGridSize(const I32x2& gridSize)
{
	mDebugGridSize = gridSize;
}

void SetDebugGridOffset(const F32x3& posOffset)
{
	mDebugGridPosOffset = posOffset;
}

void GaussianBlur(CommandList cmd, Texture2D& input, Texture2D& temp, Texture2D& output)
{
	auto& desc = input.GetDesc();
	ComputeShaderType csType = ComputeShaderType_Count;
	switch (desc.mFormat)
	{
	case FORMAT_R8_UNORM:
		csType = ComputeShaderType_BilateralBlur;
		break;
	default:
		return;
		break;
	}

	if (csType == ComputeShaderType_Count) {
		return;
	}

	mGraphicsDevice->BeginEvent(cmd, "GaussianBlur");
	mGraphicsDevice->BindComputeShader(cmd, mRenderPreset->GetComputeShader(csType));

	GPUBuffer& postprocessBuffer = mRenderPreset->GetConstantBuffer(ConstantBufferType_Postprocess);
	mGraphicsDevice->BindConstantBuffer(cmd, SHADERSTAGES_CS, postprocessBuffer, CB_GETSLOT_NAME(PostprocessCB));

	// 将blur分解成两次不同方向上的blur
	// Horizontal:
	{
		PostprocessCB cb;
		cb.gPPResolution.x = desc.mWidth;
		cb.gPPResolution.y = desc.mHeight;
		cb.gPPInverseResolution.x = (1.0f / cb.gPPResolution.x);
		cb.gPPInverseResolution.y = (1.0f / cb.gPPResolution.y);

		mGraphicsDevice->UpdateBuffer(cmd, postprocessBuffer, &cb, sizeof(PostprocessCB));
		mGraphicsDevice->BindGPUResource(cmd, SHADERSTAGES_CS, &input, TEXTURE_SLOT_UNIQUE_0);
		mGraphicsDevice->BindUAV(cmd, &temp, 0);
		mGraphicsDevice->Dispatch(
			cmd, 
			(U32)((desc.mWidth + SHADER_GAUSSIAN_BLUR_BLOCKSIZE - 1) / SHADER_GAUSSIAN_BLUR_BLOCKSIZE),
			desc.mHeight,
			1
		);
		mGraphicsDevice->UnBindUAVs(cmd, 0, 1);
	}
	// Vertical:
	{
		PostprocessCB cb;
		cb.gPPResolution.x = desc.mWidth;
		cb.gPPResolution.y = desc.mHeight;
		cb.gPPInverseResolution.x = (1.0f / cb.gPPResolution.x);
		cb.gPPInverseResolution.y = (1.0f / cb.gPPResolution.y);

		mGraphicsDevice->UpdateBuffer(cmd, postprocessBuffer, &cb, sizeof(PostprocessCB));
		mGraphicsDevice->BindGPUResource(cmd, SHADERSTAGES_CS, &temp, TEXTURE_SLOT_UNIQUE_0);
		mGraphicsDevice->BindUAV(cmd, &output, 0);
		mGraphicsDevice->Dispatch(
			cmd, 
			desc.mWidth,
			(U32)((desc.mHeight + SHADER_GAUSSIAN_BLUR_BLOCKSIZE - 1) / SHADER_GAUSSIAN_BLUR_BLOCKSIZE),
			1
		);
		mGraphicsDevice->UnBindUAVs(cmd, 0, 1);
	}

	mGraphicsDevice->EndEvent(cmd);
}

void BilateralBlur(CommandList cmd, Texture2D& input, Texture2D& temp, Texture2D& output, Texture2D& linearDepthBuffer, F32 depthThreshold)
{
	auto& desc = input.GetDesc();
	ComputeShaderType csType = ComputeShaderType_Count;
	switch (desc.mFormat)
	{
	case FORMAT_R8_UNORM:
		csType = ComputeShaderType_BilateralBlur;
		break;
	default:
		return;
		break;
	}

	if (csType == ComputeShaderType_Count) {
		return;
	}

	mGraphicsDevice->BeginEvent(cmd, "BilateralBlur");
	mGraphicsDevice->BindComputeShader(cmd, mRenderPreset->GetComputeShader(csType));
	mGraphicsDevice->BindGPUResource(cmd, SHADERSTAGES_CS, &linearDepthBuffer, TEXTURE_SLOT_LINEAR_DEPTH);

	GPUBuffer& postprocessBuffer = mRenderPreset->GetConstantBuffer(ConstantBufferType_Postprocess);
	mGraphicsDevice->BindConstantBuffer(cmd, SHADERSTAGES_CS, postprocessBuffer, CB_GETSLOT_NAME(PostprocessCB));

	// 将blur分解成两次不同方向上的blur
	// Horizontal:
	{
		PostprocessCB cb;
		cb.gPPResolution.x = desc.mWidth;
		cb.gPPResolution.y = desc.mHeight;
		cb.gPPInverseResolution.x = (1.0f / cb.gPPResolution.x);
		cb.gPPInverseResolution.y = (1.0f / cb.gPPResolution.y);
		cb.gPPParam1 = depthThreshold;
		cb.gPPParam2 = 1;
		cb.gPPParam3 = 0;

		mGraphicsDevice->UpdateBuffer(cmd, postprocessBuffer, &cb, sizeof(PostprocessCB));
		mGraphicsDevice->BindGPUResource(cmd, SHADERSTAGES_CS, &input, TEXTURE_SLOT_UNIQUE_0);
		mGraphicsDevice->BindUAV(cmd, &temp, 0);
		mGraphicsDevice->Dispatch(
			cmd, 
			(U32)((desc.mWidth + SHADER_GAUSSIAN_BLUR_BLOCKSIZE - 1) / SHADER_GAUSSIAN_BLUR_BLOCKSIZE),
			desc.mHeight,
			1
		);
		mGraphicsDevice->UnBindUAVs(cmd, 0, 1);
	}
	// Vertical:
	{
		PostprocessCB cb;
		cb.gPPResolution.x = desc.mWidth;
		cb.gPPResolution.y = desc.mHeight;
		cb.gPPInverseResolution.x = (1.0f / cb.gPPResolution.x);
		cb.gPPInverseResolution.y = (1.0f / cb.gPPResolution.y);
		cb.gPPParam1 = depthThreshold;
		cb.gPPParam2 = 0;
		cb.gPPParam3 = 1;

		mGraphicsDevice->UpdateBuffer(cmd, postprocessBuffer, &cb, sizeof(PostprocessCB));
		mGraphicsDevice->BindGPUResource(cmd, SHADERSTAGES_CS, &temp, TEXTURE_SLOT_UNIQUE_0);
		mGraphicsDevice->BindUAV(cmd, &output, 0);
		mGraphicsDevice->Dispatch(
			cmd, 
			desc.mWidth,
			(U32)((desc.mHeight + SHADER_GAUSSIAN_BLUR_BLOCKSIZE - 1) / SHADER_GAUSSIAN_BLUR_BLOCKSIZE),
			1
		);
		mGraphicsDevice->UnBindUAVs(cmd, 0, 1);
	}

	mGraphicsDevice->EndEvent(cmd);
}

void UpsampleBilateral(CommandList cmd, Texture2D& input, Texture2D& output, Texture2D& linearDepthBuffer, F32 depthThreshold)
{
	auto& desc = output.GetDesc();
	ComputeShaderType csType = ComputeShaderType_Count;
	switch (desc.mFormat)
	{
	case FORMAT_R8_UNORM:
		csType = ComputeShaderType_BilateralUpsample;
		break;
	default:
		return;
		break;
	}
	if (csType == ComputeShaderType_Count) {
		return;
	}

	mGraphicsDevice->BeginEvent(cmd, "UnsampleBilateral");
	mGraphicsDevice->BindComputeShader(cmd, mRenderPreset->GetComputeShader(csType));
	mGraphicsDevice->BindGPUResource(cmd, SHADERSTAGES_CS, &linearDepthBuffer, TEXTURE_SLOT_LINEAR_DEPTH);
	
	GPUBuffer& postprocessBuffer = mRenderPreset->GetConstantBuffer(ConstantBufferType_Postprocess);
	PostprocessCB cb;
	cb.gPPResolution.x = desc.mWidth;
	cb.gPPResolution.y = desc.mHeight;
	cb.gPPInverseResolution.x = (1.0f / cb.gPPResolution.x);
	cb.gPPInverseResolution.y = (1.0f / cb.gPPResolution.y);

	auto& inputDesc = input.GetDesc();
	cb.gPPParam1 = (1.0f / inputDesc.mWidth);
	cb.gPPParam2 = (1.0f / inputDesc.mHeight);
	cb.gPPParam3 = depthThreshold;

	mGraphicsDevice->UpdateBuffer(cmd, postprocessBuffer, &cb, sizeof(PostprocessCB));
	mGraphicsDevice->BindConstantBuffer(cmd, SHADERSTAGES_CS, postprocessBuffer, CB_GETSLOT_NAME(PostprocessCB));
	mGraphicsDevice->BindGPUResource(cmd, SHADERSTAGES_CS, &input, TEXTURE_SLOT_UNIQUE_0);
	mGraphicsDevice->BindUAV(cmd, &output, 0);
	mGraphicsDevice->Dispatch(
		cmd,
		(U32)((desc.mWidth  + SHADER_POSTPROCESS_BLOCKSIZE - 1) / SHADER_POSTPROCESS_BLOCKSIZE),
		(U32)((desc.mHeight + SHADER_POSTPROCESS_BLOCKSIZE - 1) / SHADER_POSTPROCESS_BLOCKSIZE),
		1
	);
	mGraphicsDevice->UnBindUAVs(cmd, 0, 1);
	mGraphicsDevice->EndEvent(cmd);
}

void PostprocessTonemap(CommandList cmd, Texture2D& input, Texture2D& output, F32 exposure)
{
	mGraphicsDevice->BeginEvent(cmd, "Postprocess_Tonemap");

	mGraphicsDevice->BindRenderTarget(cmd, 0, nullptr, nullptr);
	mGraphicsDevice->BindComputeShader(cmd, mRenderPreset->GetComputeShader(ComputeShaderType_Tonemapping));

	// bind input texture
	mGraphicsDevice->BindGPUResource(cmd, SHADERSTAGES_CS, &input, TEXTURE_SLOT_UNIQUE_0);

	// bind post process buffer
	const TextureDesc desc = output.GetDesc();
	PostprocessCB cb;
	cb.gPPResolution.x = desc.mWidth;
	cb.gPPResolution.y = desc.mHeight;
	cb.gPPInverseResolution.x = (1.0f / desc.mWidth);
	cb.gPPInverseResolution.y = (1.0f / desc.mHeight);
	cb.gPPParam1 = exposure;

	GPUBuffer& postprocessBuffer = mRenderPreset->GetConstantBuffer(ConstantBufferType_Postprocess);
	mGraphicsDevice->UpdateBuffer(cmd, postprocessBuffer, &cb, sizeof(PostprocessCB));
	mGraphicsDevice->BindConstantBuffer(cmd, SHADERSTAGES_CS, postprocessBuffer, CB_GETSLOT_NAME(PostprocessCB));

	// bind output texture
	GPUResource* uavs[] = { &output };
	mGraphicsDevice->BindUAVs(cmd, uavs, 0, 1);
	mGraphicsDevice->Dispatch(
		cmd,
		(U32)((desc.mWidth + SHADER_POSTPROCESS_BLOCKSIZE - 1) / SHADER_POSTPROCESS_BLOCKSIZE),
		(U32)((desc.mHeight + SHADER_POSTPROCESS_BLOCKSIZE - 1) / SHADER_POSTPROCESS_BLOCKSIZE),
		1
	);
	mGraphicsDevice->UnBindUAVs(cmd, 0, 1);
	mGraphicsDevice->EndEvent(cmd);
}

void PostprocessFXAA(CommandList cmd, Texture2D& input, Texture2D& output)
{
	mGraphicsDevice->BeginEvent(cmd, "Postprocess_FXAA");

	//BindConstanceBuffer(SHADERSTAGES_CS);

	mGraphicsDevice->BindRenderTarget(cmd, 0, nullptr, nullptr);
	mGraphicsDevice->BindComputeShader(cmd, mRenderPreset->GetComputeShader(ComputeShaderType_FXAA));

	// bind input texture
	mGraphicsDevice->BindGPUResource(cmd, SHADERSTAGES_CS, &input, TEXTURE_SLOT_UNIQUE_0);

	// bind post process buffer
	const TextureDesc desc = output.GetDesc();
	PostprocessCB cb;
	cb.gPPResolution.x = desc.mWidth;
	cb.gPPResolution.y = desc.mHeight;
	cb.gPPInverseResolution.x = (1.0f / desc.mWidth);
	cb.gPPInverseResolution.y = (1.0f / desc.mHeight);

	GPUBuffer& postprocessBuffer = mRenderPreset->GetConstantBuffer(ConstantBufferType_Postprocess);
	mGraphicsDevice->UpdateBuffer(cmd, postprocessBuffer, &cb, sizeof(PostprocessCB));
	mGraphicsDevice->BindConstantBuffer(cmd, SHADERSTAGES_CS, postprocessBuffer, CB_GETSLOT_NAME(PostprocessCB));

	// bind output texture
	GPUResource* uavs[] = { &output };
	mGraphicsDevice->BindUAVs(cmd, uavs, 0, 1);
	mGraphicsDevice->Dispatch(
		cmd, 
		(U32)((desc.mWidth + SHADER_POSTPROCESS_BLOCKSIZE - 1) / SHADER_POSTPROCESS_BLOCKSIZE),
		(U32)((desc.mHeight + SHADER_POSTPROCESS_BLOCKSIZE - 1) / SHADER_POSTPROCESS_BLOCKSIZE),
		1
	);
	mGraphicsDevice->UnBindUAVs(cmd, 0, 1);
	mGraphicsDevice->EndEvent(cmd);
}

void BindCommonResource(CommandList cmd)
{
	const std::pair<SamplerStateID, U32> stateIDs[] = {
		{ SamplerStateID::SamplerStateID_PointClamp,          SAMPLER_POINT_CLAMP_SLOT },
		{ SamplerStateID::SamplerStateID_LinearClamp,         SAMPLER_LINEAR_CLAMP_SLOT },
		{ SamplerStateID::SamplerStateID_Anisotropic,         SAMPLER_ANISOTROPIC_SLOT },
		{ SamplerStateID::SamplerStateID_ComparisionDepth,    SAMPLER_COMPARISON_SLOT },
		{ SamplerStateID::SamplerStateID_Object,			  SAMPLER_OBJECT_SLOT },
	};
	for (int stageIndex = 0; stageIndex < SHADERSTAGES_COUNT; stageIndex++)
	{
		SHADERSTAGES stage = static_cast<SHADERSTAGES>(stageIndex);
		for (const auto& kvp : stateIDs)
		{
			auto sampler = mRenderPreset->GetSamplerState(kvp.first);
			mGraphicsDevice->BindSamplerState(cmd, stage, *sampler, kvp.second);
		}

		BindConstanceBuffer(cmd, stage);
	}

	// bind shader light resource
	GPUResource* resources[] = {
		&mRenderPreset->GetStructuredBuffer(StructuredBufferType_ShaderLight),
		&mRenderPreset->GetStructuredBuffer(StructuredBufferType_MatrixArray)
	};
	mGraphicsDevice->BindGPUResources(cmd, SHADERSTAGES_PS, resources, SBSLOT_SHADER_LIGHT_ARRAY, ARRAYSIZE(resources));
	mGraphicsDevice->BindGPUResources(cmd, SHADERSTAGES_CS, resources, SBSLOT_SHADER_LIGHT_ARRAY, ARRAYSIZE(resources));
}

void UpdateCameraCB(CommandList cmd, CameraComponent & camera)
{
	CameraCB cb;

	DirectX::XMStoreFloat4x4(&cb.gCameraVP,    camera.GetViewProjectionMatrix());
	DirectX::XMStoreFloat4x4(&cb.gCameraView,  camera.GetViewMatrix());
	DirectX::XMStoreFloat4x4(&cb.gCameraProj,  camera.GetProjectionMatrix());
	DirectX::XMStoreFloat4x4(&cb.gCameraInvV,  camera.GetInvViewMatrix());
	DirectX::XMStoreFloat4x4(&cb.gCameraInvP,  camera.GetInvProjectionMatrix());
	DirectX::XMStoreFloat4x4(&cb.gCameraInvVP, camera.GetInvViewProjectionMatrix());

	cb.gCameraPos = XMConvert(camera.GetCameraPos());
	cb.gCameraNearZ = camera.GetNearPlane();
	cb.gCameraFarZ = camera.GetFarPlane();
	cb.gCameraInvNearZ = (1.0f / std::max(0.00001f, cb.gCameraNearZ));
	cb.gCameraInvFarZ = (1.0f / std::max(0.00001f, cb.gCameraFarZ));

	GPUBuffer& cameraBuffer = mRenderPreset->GetConstantBuffer(ConstantBufferType_Camera);
	GetDevice().UpdateBuffer(cmd, cameraBuffer, &cb, sizeof(CameraCB));
}

void UpdateFrameCB(CommandList cmd)
{
	FrameCB frameCB;
	frameCB.gFrameScreenSize = XMConvert(mFrameData.mFrameScreenSize);
	frameCB.gFrameInvScreenSize = XMConvert(mFrameData.mFrameInvScreenSize);
	frameCB.gFrameAmbient = XMConvert(mFrameData.mFrameAmbient);
	frameCB.gShaderLightArrayCount = mFrameData.mShaderLightArrayCount;
	frameCB.gFrameGamma = GetGamma();
	frameCB.gFrameShadowCascadeCount = shadowCascadeCount;
	frameCB.gFrameTileCullingCount = XMConvert(GetCullingTiledCount());
	frameCB.gFrameTime = mFrameData.mFrameTime;
	frameCB.gFrameDeltaTime = mFrameData.mFrameDeltaTime;

	GPUBuffer& frameBuffer = mRenderPreset->GetConstantBuffer(ConstantBufferType_Frame);
	mGraphicsDevice->UpdateBuffer(cmd, frameBuffer, &frameCB, sizeof(FrameCB));
}

Scene & GetMainScene()
{
	return Scene::GetScene();
}

PipelineStateManager & GetPipelineStateManager()
{
	return *mPipelineStateManager;
}

RenderPath* GetRenderPath()
{
	return mCurrentRenderPath.get();
}

void SetCurrentRenderPath(RenderPath * renderPath)
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

void AddDeferredTextureMipGen(Texture2D& texture)
{
	if (mDeferredMIPGenerator == nullptr) {
		Debug::Warning("Invalid deferred texture mip generator.");
		return;
	}

	mDeferredMIPGenerator->AddTexture(texture);
}

void RegisterRenderPass(const StringID& name, std::shared_ptr<RenderPass> renderPath)
{
	mRenderPassMap[name] = renderPath; 
}

std::shared_ptr<RenderPass> GetRenderPass(const StringID& name)
{
	auto it = mRenderPassMap.find(name);
	if (it != mRenderPassMap.end()) {
		return it->second;
	}
	return nullptr;
}

TerrainTreePtr RegisterTerrain(ECS::Entity entity, U32 width, U32 height, U32 elevation)
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

void UnRegisterTerrain(ECS::Entity entity)
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

TerrainTreePtr GetTerrainTree(ECS::Entity entity)
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

void ReloadShaders()
{
	mShaderLib->Reload();
}

ShaderPtr LoadShader(SHADERSTAGES stages, const std::string& path)
{
	return mShaderLib->LoadShader(stages, path);
}

VertexShaderInfo LoadVertexShaderInfo(const std::string& path, VertexLayoutDesc* desc, U32 numElements)
{
	return mShaderLib->LoadVertexShaderInfo(path, desc, numElements);
}

void RegisterCustomPipelineState(RenderPassType passType, const StringID& name, PipelineStateDesc desc)
{
	mPipelineStateManager->RegisterCustomPipelineState(passType, name, desc);
}

PipelineState* GetPipelineStateByStringID(RenderPassType passType, const StringID& id)
{
	return mPipelineStateManager->GetCustomPipelineState(passType, id);
}

void InitializeRenderPasses()
{
	// terrain
	std::shared_ptr<RenderPass> terrainPass = CJING_MAKE_SHARED<TerrainPass>();
	terrainPass->Initialize();
	RegisterRenderPass(STRING_ID(TerrainPass), terrainPass);

	// particle
	std::shared_ptr<RenderPass> particlePass = CJING_MAKE_SHARED<ParticlePass>();
	particlePass->Initialize();
	RegisterRenderPass(STRING_ID(ParticlePass), particlePass);
}

void UninitializeRenderPasses()
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

void ProcessRenderQueue(CommandList cmd, RenderQueue & queue, RenderPassType renderPassType, RenderableType renderableType, U32 instanceReplicator, InstanceHandler* instanceHandler)
{
	if (queue.IsEmpty() == true) {
		return;
	}

	LinearAllocator& renderAllocator = GetRenderAllocator(cmd);
	auto& scene = GetMainScene();

	size_t instanceSize = sizeof(RenderInstance);
	size_t allocSize = queue.GetBatchCount() * instanceReplicator * instanceSize;
	GraphicsDevice::GPUAllocation instances = mGraphicsDevice->AllocateGPU(cmd, allocSize);

	// 对于使用相同mesh的object将创建同一个instanceBatch，包含相同的shader，
	// 但是依然会为每个object创建一个instance（input layout）包含了颜色和世界坐标变换矩阵

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

			RenderBatchInstance* batchInstance = (RenderBatchInstance*)renderAllocator.Allocate(sizeof(RenderBatchInstance));
			batchInstance->mMeshEntity = meshEntity;
			batchInstance->mDataOffset = instances.offset + currentInstanceCount * instanceSize;
			batchInstance->mInstanceCount = 0;

			mRenderBatchInstances.push_back(batchInstance);
		}

		const auto object = scene.mObjects.GetComponent(objectEntity);
		const auto transform = scene.mTransforms.GetComponent(objectEntity);
		const auto aabb = scene.mObjectAABBs.GetComponent(objectEntity);

		// 保存每个batch（包含一个object）的worldMatrix
		// 用于在shader中计算顶点的世界坐标
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
			
			// TODO: maybe there is a better way to set userdata
			if (object->RenderInstanceHandler != nullptr) {
				object->RenderInstanceHandler(((RenderInstance*)instances.data)[currentInstanceCount]);
			}

			// set userdata
			if (instanceHandler != nullptr &&
				instanceHandler->processInstance_ != nullptr) 
			{
				instanceHandler->processInstance_(subIndex, ((RenderInstance*)instances.data)[currentInstanceCount]);
			}

			// 当前batchInstance必然在array最后
			mRenderBatchInstances.back()->mInstanceCount++;
			currentInstanceCount++;
		}
	}

	// simpleBindTexture，只绑定baseColorMap
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
		if (mesh->IsEmpty()) {
			continue;
		}

		mGraphicsDevice->BindIndexBuffer(cmd, *mesh->GetIndexBuffer(), mesh->GetIndexFormat(), 0);

		BoundVexterBufferType prevBoundType = BoundVexterBufferType_Nothing;
		for (auto& subset : mesh->GetSubsets())
		{
			if (subset.mIndexCount <= 0) {
				continue;
			}

			auto material = scene.GetComponent<MaterialComponent>(subset.mMaterialID);
			if (material == nullptr) {
				continue;
			}

			// TODO: 很多变量和设置可以整合到PipelineState中
			PipelineState* pso = nullptr;

			// TODO; 优化customShader
			auto customShaderID = material->GetCustomShader();
			if (customShaderID != StringID::EMPTY)
			{
				pso = mPipelineStateManager->GetCustomPipelineState(renderPassType, customShaderID);
				if (pso == nullptr) {
					pso = mPipelineStateManager->GetNormalPipelineState(renderPassType, *material);
				}
			}
			else
			{
				pso = mPipelineStateManager->GetNormalPipelineState(renderPassType, *material);
			}

			if (pso == nullptr || !pso->IsValid()) {
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

				// 根据不同的BoundType传递不同格式的vertexBuffer
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
							bathInstance->mDataOffset	// 因为所有的batch公用一个buffer，所以提供offset
						};
						mGraphicsDevice->BindVertexBuffer(cmd, vbs, 0, ARRAYSIZE(vbs), strides, offsets);
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
							bathInstance->mDataOffset	// 因为所有的batch公用一个buffer，所以提供offset
						};
						mGraphicsDevice->BindVertexBuffer(cmd, vbs, 0, ARRAYSIZE(vbs), strides, offsets);
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
							bathInstance->mDataOffset	// 因为所有的batch公用一个buffer，所以提供offset
						};
						mGraphicsDevice->BindVertexBuffer(cmd, vbs, 0, ARRAYSIZE(vbs), strides, offsets);
					}
					break;
				}
			}

			// set alpha cut ref
			SetAlphaCutRef(cmd, material->GetAlphaCutRef());

			// bind material state
			mGraphicsDevice->BindPipelineState(cmd, pso);

			// bind material constant buffer
			mGraphicsDevice->BindConstantBuffer(cmd, SHADERSTAGES_VS, material->GetConstantBuffer(), CB_GETSLOT_NAME(MaterialCB));
			mGraphicsDevice->BindConstantBuffer(cmd, SHADERSTAGES_PS, material->GetConstantBuffer(), CB_GETSLOT_NAME(MaterialCB));

			// bind material texture
			if (simpleBindTexture)
			{
				const GPUResource* resources[] = {
					material->GetBaseColorMap(),
				};
				mGraphicsDevice->BindGPUResources(cmd, SHADERSTAGES_PS, resources, TEXTURE_SLOT_0, 1);
			}
			else
			{
				const GPUResource* resources[] = {
					material->GetBaseColorMap(),
					material->GetNormalMap(),
					material->GetSurfaceMap(),
				};
				mGraphicsDevice->BindGPUResources(cmd, SHADERSTAGES_PS, resources, TEXTURE_SLOT_0, 3);
			}

			// draw
			mGraphicsDevice->DrawIndexedInstanced(cmd, subset.mIndexCount, bathInstance->mInstanceCount, subset.mIndexOffset, 0, 0);
		}
	}

	// remove alpha test
	ResetAlphaCutRef(cmd);

	mGraphicsDevice->UnAllocateGPU(cmd);	
	renderAllocator.Free(instancedBatchCount * sizeof(RenderBatchInstance));
}

void BindConstanceBuffer(CommandList cmd, SHADERSTAGES stage)
{
	mGraphicsDevice->BindConstantBuffer(cmd, stage, mRenderPreset->GetConstantBuffer(ConstantBufferType_Common), CB_GETSLOT_NAME(CommonCB));
	mGraphicsDevice->BindConstantBuffer(cmd, stage, mRenderPreset->GetConstantBuffer(ConstantBufferType_Camera), CB_GETSLOT_NAME(CameraCB));
	mGraphicsDevice->BindConstantBuffer(cmd, stage, mRenderPreset->GetConstantBuffer(ConstantBufferType_Frame),  CB_GETSLOT_NAME(FrameCB));
}

void BindShadowMaps(CommandList cmd, SHADERSTAGES stage)
{
	mGraphicsDevice->BindGPUResource(cmd, stage, &shadowMapArray2D,   TEXTURE_SLOT_SHADOW_ARRAY_2D);
	mGraphicsDevice->BindGPUResource(cmd, stage, &shadowMapArrayCube, TEXTURE_SLOT_SHADOW_ARRAY_CUBE);
}

void RenderDirLightShadowmap(CommandList cmd, LightComponent& light, CameraComponent& camera, U32 renderLayerMask)
{
	// 基于CSM的方向光阴影
	// 1.将视锥体分成CASCADE_COUNT个子视锥体，对于每个子视锥体创建包围求和变换矩阵
	std::vector<LightShadowCamera> cascadeShadowCameras;
	CreateCascadeShadowCameras(light, camera, shadowCascadeCount, cascadeShadowCameras);

	LinearAllocator& RenderAllocator = GetRenderAllocator(cmd);
	Scene& mainScene = GetMainScene();
	GraphicsDevice& device = GetDevice();
	GPUBuffer& cameraBuffer = mRenderPreset->GetConstantBuffer(ConstantBufferType_Camera);

	// 2.根据级联视锥体创建的包围盒，获取每个级联对应的物体
	for (U32 cascadeLevel = 0; cascadeLevel < shadowCascadeCount; cascadeLevel++)
	{
		LightShadowCamera& cam = cascadeShadowCameras[cascadeLevel];
		RenderQueue renderQueue;

		for (U32 index = 0; index < mainScene.mObjectAABBs.GetCount(); index++)
		{
			auto aabb = mainScene.mObjectAABBs[index];
			if (aabb == nullptr) {
				continue;
			}

			auto layer = mainScene.mLayers.GetComponent(aabb->GetCurrentEntity());
			if (layer != nullptr && !(layer->GetLayerMask() & renderLayerMask)) {
				continue;
			}

			if (cam.mFrustum.Overlaps(aabb->GetAABB()))
			{
				auto object = mainScene.mObjects[index];
				if (object == nullptr || 
					object->IsRenderable() == false ||
					object->IsCastShadow() == false ||
					object->mShadowCascadeMask > cascadeLevel ) 
				{
					continue;
				}

				RenderBatch* renderBatch = (RenderBatch*)RenderAllocator.Allocate(sizeof(RenderBatch));
				renderBatch->Init(object->GetCurrentEntity(), object->mMeshID, 0);
				renderQueue.AddBatch(renderBatch);
			}
		}

		I32 resourceIndex = light.GetShadowMapIndex() + cascadeLevel;
		// 对于每个级联绘制包含物体的深度贴图
		if (!renderQueue.IsEmpty())
		{
			shadowMapDirtyTable[resourceIndex] = true;

			CameraCB cb;
			DirectX::XMStoreFloat4x4(&cb.gCameraVP, cam.GetViewProjectionMatrix());
			device.UpdateBuffer(cmd, cameraBuffer, &cb, sizeof(CameraCB));

			ViewPort vp = {};
			vp.mWidth = (F32)shadowRes2DResolution;
			vp.mHeight = (F32)shadowRes2DResolution;
			vp.mMinDepth = 0.0f;
			vp.mMaxDepth = 1.0f;
			device.BindViewports(cmd, &vp, 1);

			device.BeginRenderBehavior(cmd, shadowRenderBehaviors[resourceIndex]);
			ProcessRenderQueue(cmd, renderQueue, RenderPassType_Shadow, RenderableType_Opaque);
			device.EndRenderBehavior(cmd);

			RenderAllocator.Free(renderQueue.GetBatchCount() * sizeof(RenderBatch));
		}
		else if (resourceIndex >= 0 && shadowMapDirtyTable[resourceIndex])
		{
			shadowMapDirtyTable[resourceIndex] = false;
			device.ClearDepthStencil(cmd, shadowMapArray2D, CLEAR_DEPTH, 0.0f, 0, resourceIndex);
		}
	}
}

void RenderPointLightShadowmap(CommandList cmd, LightComponent& light, CameraComponent& camera, U32 renderLayerMask)
{
	if (!GetDevice().CheckGraphicsFeatureSupport(GraphicsFeatureSupport::VIEWPORT_AND_RENDERTARGET_ARRAYINDEX_WITHOUT_GS)) 
	{
		Debug::Warning("RenderPointLightShadowmap failed: Feature not support: VIEWPORT_AND_RENDERTARGET_ARRAYINDEX_WITHOUT_GS");
		return;
	}

	LinearAllocator& renderAllocator = GetRenderAllocator(cmd);
	RenderQueue renderQueue;

	// 点光源根据范围创建球状包围盒
	Sphere lightSphere(XMConvert(light.mPosition), light.mRange);
	Scene& mainScene = GetMainScene();
	for (U32 index = 0; index < mainScene.mObjectAABBs.GetCount(); index++)
	{
		auto aabb = mainScene.mObjectAABBs[index];
		if (aabb == nullptr) {
			continue;
		}

		auto layer = mainScene.mLayers.GetComponent(aabb->GetCurrentEntity());
		if (layer != nullptr && !(layer->GetLayerMask() & renderLayerMask)) {
			continue;
		}

		if (lightSphere.Intersects(aabb->GetAABB()))
		{
			auto object = mainScene.mObjects[index];
			if (object == nullptr ||
				object->IsRenderable() == false ||
				object->IsCastShadow() == false)
			{
				continue;
			}

			RenderBatch* renderBatch = (RenderBatch*)renderAllocator.Allocate(sizeof(RenderBatch));
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

		// 创建6个面的光源相机变换投影矩阵，顺序为x,-x,y, -y, z,-z
		// light default dir is (0, -1, 0), up(0, 0, 1)
		// q.x = sin(theta / 2) * axis.x
		// q.y = sin(theta / 2) * axis.y
		// q.z = sin(theta / 2) * axis.z
		// q.w = cos(theta / 2)
		// 或者通过欧拉角转换得到四元数
		const LightShadowCamera cams[] = {
			LightShadowCamera(lightPos, XMVectorSet(0.5f, -0.5f, -0.5f, -0.5f), nearZ, farZ, XM_PIDIV2), //  x
			LightShadowCamera(lightPos, XMVectorSet(0.5f, 0.5f, 0.5f, -0.5f), nearZ, farZ, XM_PIDIV2),   // -x
			LightShadowCamera(lightPos, XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), nearZ, farZ, XM_PIDIV2),    //  y
			LightShadowCamera(lightPos, XMVectorSet(0.0f, 0.0f, 0.0f, -1.0f), nearZ, farZ, XM_PIDIV2),   // -y
			LightShadowCamera(lightPos, XMVectorSet(0.707f, 0, 0, -0.707f), nearZ, farZ, XM_PIDIV2),     //  z, 0.707 = 根号2/2
			LightShadowCamera(lightPos, XMVectorSet(0, 0.707f, 0.707f, 0), nearZ, farZ, XM_PIDIV2),      // -z
		};

		// 对每个方向的椎体和当前相机椎体相交测试，以排除无需渲染的方向
		DirectX::BoundingFrustum camFrustum = CreateBoundingFrustum(camera.GetProjectionMatrix());
		TransformBoundingFrustum(camFrustum, camera.GetInvViewMatrix());

		size_t availableCount = 0;
		Frustum availableCams[6] = {};
		CubemapRenderCB cb;
		for (U32 i = 0; i < ARRAYSIZE(cams); i++)
		{
			if (camFrustum.Intersects(cams[i].mBoundingFrustum))
			{
				XMStoreFloat4x4(&cb.gCubemapRenderCams[availableCount].cubemapVP, cams[i].mViewProjection);
				cb.gCubemapRenderCams[availableCount].properties = XMConvert(U32x4(i, 0u, 0u, 0u));
				availableCams[availableCount] = cams[i].mFrustum;
				availableCount++;
			}
		}
		auto buffer = mRenderPreset->GetConstantBuffer(ConstantBufferType_CubeMap);
		mGraphicsDevice->UpdateBuffer(cmd, buffer, &cb, sizeof(CubemapRenderCB));
		mGraphicsDevice->BindConstantBuffer(cmd, SHADERSTAGES_VS, buffer, CB_GETSLOT_NAME(CubemapRenderCB));

		ViewPort vp;
		vp.mWidth = (F32)shadowResCubeResolution;
		vp.mHeight = (F32)shadowResCubeResolution;
		vp.mMinDepth = 0.0f;
		vp.mMaxDepth = 1.0f;
		device.BindViewports(cmd, &vp, 1);

		// instance handler
		InstanceHandler instanceHandler;
		instanceHandler.checkCondition_ = [availableCams](U32 subIndex, ECS::Entity entity, Scene& scene)
		{
			auto aabb = scene.mObjectAABBs.GetComponent(entity);
			if (aabb == nullptr) {
				return false;
			}
			return availableCams[subIndex].Overlaps(aabb->GetAABB());
		};
		instanceHandler.processInstance_ = [availableCams](U32 subIndex, RenderInstance& instance)
		{
			instance.userdata.y = subIndex;
		};

		// rendering
		device.BeginRenderBehavior(cmd, shadowCubeRenderBehaviors[resourceIndex]);
		ProcessRenderQueue(cmd, renderQueue, RenderPassType_ShadowCube, RenderableType_Opaque, availableCount, &instanceHandler);
		device.EndRenderBehavior(cmd);

		renderAllocator.Free(renderQueue.GetBatchCount() * sizeof(RenderBatch));
	}
	else if (resourceIndex >=0 && shadowCubeMapDirtyTable[resourceIndex])
	{
		shadowCubeMapDirtyTable[resourceIndex] = false;
		device.ClearDepthStencil(cmd, shadowMapArrayCube, CLEAR_DEPTH, 0.0f, 0, resourceIndex);
	}
}

void ProcessSkinning(CommandList cmd)
{
	PROFILER_BEGIN_GPU_BLOCK(cmd, "Skinning");
	mGraphicsDevice->BeginEvent(cmd, "Skinning");
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
			// 因为需要写入到顶点缓冲中，所以需要清除下VertexBuffer
			mGraphicsDevice->ClearVertexBuffer(cmd);
			mGraphicsDevice->BindComputeShader(cmd, mRenderPreset->GetComputeShader(ComputeShaderType_Skinning));
		}

		// update bonePoses buffer
		mGraphicsDevice->UpdateBuffer(cmd, armature->mBufferBonePoses, armature->mBonePoses.data(), sizeof(ArmatureComponent::BonePose) * armature->mBonePoses.size());
		mGraphicsDevice->BindGPUResource(cmd, SHADERSTAGES_CS, &armature->mBufferBonePoses, SKINNING_SBSLOT_BONE_POSE_BUFFER);

		GPUResource* resoureces[] = { 
			mesh.GetVertexBufferPos(),
			mesh.GetVertexBufferBoneIndexWeight(),
		};
		GPUResource* uavs[] = {
			mesh.GetVertexBufferStreamoutPos()
		};
		mGraphicsDevice->BindGPUResources(cmd, SHADERSTAGES_CS, resoureces, SKINNING_SLOT_VERTEX_POS, ARRAYSIZE(resoureces));
		mGraphicsDevice->BindUAVs(cmd, uavs, 0, 1);
		mGraphicsDevice->Dispatch(
			cmd,
			(U32)((mesh.mVertexPositions.size() + SHADER_SKINNING_BLOCKSIZE - 1) / SHADER_SKINNING_BLOCKSIZE),
			1,
			1
		);
	}

	if (isResourceSetup)
	{
		mGraphicsDevice->UnBindUAVs(cmd, 0, 1);
		mGraphicsDevice->UnbindGPUResources(cmd, SKINNING_SLOT_VERTEX_POS, 2);
	}

	mGraphicsDevice->EndEvent(cmd);
	PROFILER_END_BLOCK();
}

void ProcessParticles(CommandList cmd, Scene& scene, const std::vector<U32>& culledParticles)
{
	if (culledParticles.empty()) {
		return;
	}

	PROFILER_BEGIN_GPU_BLOCK(cmd, "GPU Particles");
	for (const auto& particleIndex : culledParticles)
	{
		auto particle = scene.mParticles[particleIndex];
		if (particle == nullptr) {
			continue;
		}

		particle->UpdateGPU(cmd);
	}
	PROFILER_END_BLOCK();
}

LinearAllocator& GetFrameAllocator()
{
	if (mFrameAllocator.GetCapacity() <= 0) {
		mFrameAllocator.Reserve((size_t)MAX_FRAME_ALLOCATOR_SIZE);
	}

	return mFrameAllocator;
}

LinearAllocator& GetRenderAllocator(CommandList cmd)
{
	auto& allocator = mRenderAllocator[cmd];
	if (allocator.GetCapacity() <= 0) {
		allocator.Reserve((size_t)MAX_FRAME_ALLOCATOR_SIZE);
	}
	return allocator;
}

void FrameCullings::Clear()
{
	mCulledObjects.clear();
	mCulledLights.clear();
	mCulledParticles.clear();
}
}
}