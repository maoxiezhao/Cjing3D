#pragma once

#include "renderer\renderableCommon.h"
#include "renderer\paths\renderPath.h"
#include "renderer\rendererUtils.h"
#include "core\subSystem.hpp"
#include "utils\allocator\linearAllocator.h"

#include <unordered_map>

namespace Cjing3D
{

class GraphicsDevice;
class ShaderLib;
class RenderPreset;
class ResourceManager;
class CameraComponent;
class LightComponent;
class Scene;
class MaterialComponent;
class PipelineStateManager;
class RenderPass;
class TerrainTree;

// Renderer被设计为一个仅仅提供各种渲染方法的集合（namespace)，具体的调用方式应该由renderPath决定
namespace Renderer
{
	void Initialize(RenderingDeviceType deviceType, HWND window);
	void Uninitialize();
	void UninitializeDevice();

	void Render();
	void Compose();
	void Present();
	void FixedUpdate();
	void Update(F32 deltaTime);

	void UpdatePerFrameData(F32 deltaTime);
	void RefreshRenderData();

	// getter
	GraphicsDevice& GetDevice();
	CameraComponent& GetCamera();
	ShaderLib& GetShaderLib();
	RenderPreset& GetRenderPreset();
	Scene& GetMainScene();
	PipelineStateManager& GetPipelineStateManager();
	RenderPath* GetRenderPath();

	// base status
	void SetGamma(F32 gamma);
	F32 GetGamma();
	U32 GetShadowCascadeCount();
	U32 GetShadowRes2DResolution();
	F32x3 GetAmbientColor();
	void SetAmbientColor(F32x3 color);
	void SetAlphaCutRef(F32 alpha);
	void ResetAlphaCutRef();
	void SetScreenSize(U32 width, U32 height);
	U32x2 GetScreenSize();
	XMMATRIX GetScreenProjection();
	Ray GetMainCameraMouseRay(const U32x2& pos);

	// Render Method
	void RenderShadowmaps(CameraComponent& camera);
	void RenderSceneOpaque(CameraComponent& camera, RenderPassType renderPassType);
	void RenderSceneTransparent(CameraComponent& camera, RenderPassType renderPassType);
	void RenderImpostor(CameraComponent& camera, RenderPassType renderPassType);
	void RenderSky();
	void RenderLinearDepth(Texture2D& depthBuffer, Texture2D& linearDepthBuffer);
	void RenderSSAO(Texture2D& depthBuffer, Texture2D& linearDepthBuffer, Texture2D& aoTexture, F32 aoRange, U32 aoSampleCount);

	// blur
	void GaussianBlur(Texture2D& input, Texture2D& temp, Texture2D& output);
	void BilateralBlur(Texture2D& input, Texture2D& temp, Texture2D& output, Texture2D& linearDepthBuffer, F32 depthThreshold);
	void UpsampleBilateral(Texture2D& input, Texture2D& output, Texture2D& linearDepthBuffer, F32 depthThreshold);

	// tiled light culling
	bool IsTiledCullingDebug();
	U32x2 GetCullingTiledCount();
	void TiledLightCulling(Texture2D& depthBuffer);

	// postprocess
	void PostprocessTonemap(Texture2D& input, Texture2D& output, F32 exposure);
	void PostprocessFXAA(Texture2D& input, Texture2D& output);

	// Binding Method
	void BindCommonResource();
	void BindConstanceBuffer(SHADERSTAGES stage);
	void BindShadowMaps(SHADERSTAGES stage);

	// const buffer function
	void UpdateCameraCB(CameraComponent& camera);
	void UpdateFrameCB();

	// render path method
	void SetCurrentRenderPath(RenderPath* renderPath);

	// deferred mipmap gen
	void AddDeferredTextureMipGen(Texture2D& texture);

	// render path
	void RegisterRenderPass(const StringID& name, std::shared_ptr<RenderPass> renderPath);
	std::shared_ptr<RenderPass> GetRenderPass(const StringID& name);

	// terrain 
	std::shared_ptr<TerrainTree> RegisterTerrain(ECS::Entity entity, U32 width, U32 height, U32 elevation);
	void UnRegisterTerrain(ECS::Entity entity);
	std::shared_ptr<TerrainTree> GetTerrainTree(ECS::Entity entity);

	// debug
	void RenderDebugScene(CameraComponent& camera);
	void SetDebugGridSize(const I32x2& gridSize, const F32x2& posOffset = {0.0f, 0.0f});

	// shader
	ShaderPtr LoadShader(SHADERSTAGES stages, const std::string& path);
	VertexShaderInfo LoadVertexShaderInfo(const std::string& path, VertexLayoutDesc* desc, U32 numElements);

	void InitializeRenderPasses();
	void UninitializeRenderPasses();

	GraphicsDevice* CreateGraphicsDeviceByType(RenderingDeviceType deviceType, HWND window);
	void ProcessRenderQueue(RenderQueue& queue, RenderPassType renderPassType, RenderableType renderableType, U32 instanceReplicator = 1, InstanceHandler* instanceHandler = nullptr);
	void RenderDirLightShadowmap(LightComponent& light, CameraComponent& camera);
	void RenderPointLightShadowmap(LightComponent& light, CameraComponent& camera);
	void ProcessSkinning();

	// 当前帧的裁剪后的数据
	struct FrameCullings
	{
		Frustum mFrustum;
		std::vector<U32> mCulledObjects; // 记录渲染的object index
		std::vector<U32> mCulledLights;

		void Clear();
	};

	// 每帧所用的线性分配器
	enum FrameAllocatorType
	{
		FrameAllocatorType_Update = 0,
		FrameAllocatorType_Render,
		FrameAllocatorType_Count
	};
	LinearAllocator& GetFrameAllocator(FrameAllocatorType type);

	// 用于记录每一帧的基础数据
	class RenderFrameData
	{
	public:
		F32x2 mFrameScreenSize;
		F32x2 mFrameInvScreenSize;
		U32 mShaderLightArrayCount;
		F32x3 mFrameAmbient;

		void Clear();
	};

	// GPU Query, latency为延迟读取的数量，避免在同一帧中请求/读取同一个query
	template<I32 latency>
	class GPUQueryHandler
	{
	public:
		GPUQueryHandler() {};
		~GPUQueryHandler() { Uninitialize(); }

		void Initialize(GPUQueryDesc& desc)
		{
			if (IsInitialized()) {
				return;
			}

			for (int i = 0; i < latency; i++)
			{
				GetDevice().CreateQuery(desc, mQueries[i]);
				mActiveTable[i] = false;
			}
			currentIndex = 0;
			mIsInitialized = true;
		}

		void Uninitialize()
		{
			if (!IsInitialized()) {
				return;
			}

			for (int i = 0; i < latency; i++) {
				mQueries[i].Clear();
			}
		}

		bool IsInitialized()const
		{
			return mIsInitialized;
		}

		// 当向GPU请求信息时，则直接返回currentIndex的query
		GPUQuery* GetQueryForGPU()
		{
			mActiveTable[currentIndex] = true;
			return &mQueries[currentIndex];
		}

		// 当CPU返回数据结果时，这里总会循环请求下一个Index+1的query，
		// 即延迟latency次读取
		GPUQuery* GetQueryForCPU()
		{
			currentIndex = (currentIndex + 1) % latency;
			if (!mActiveTable[currentIndex]) {
				return nullptr;
			}

			mActiveTable[currentIndex] = false;
			return &mQueries[currentIndex];
		}
	private:
		bool mIsInitialized = false;
		
		GPUQuery mQueries[latency];
		I32 currentIndex = 0;
		bool mActiveTable[latency];
	};
};

}