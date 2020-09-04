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
class ResourceManager;
class RenderPreset;
class CameraComponent;
class LightComponent;
class Scene;
class MaterialComponent;
class PipelineStateManager;
class RenderPass;
class TerrainTree;
struct PresentConfig;

// Renderer�����Ϊһ�������ṩ������Ⱦ�����ļ��ϣ�namespace)������ĵ��÷�ʽӦ����renderPath����
namespace Renderer
{
	void Initialize();
	void Uninitialize();
	void UninitializeDevice();

	void EndFrame();
	void FixedUpdate();
	void Update(F32 deltaTime);

	void UpdatePerFrameData(F32 deltaTime, U32 renderLayerMask);
	void RefreshRenderData(CommandList cmd);

	// getter
	GraphicsDevice& GetDevice();
	CameraComponent& GetCamera();
	ShaderLib& GetShaderLib();
	RenderPreset& GetRenderPreset();
	Scene& GetMainScene();
	PipelineStateManager& GetPipelineStateManager();
	RenderPath* GetRenderPath();

	// base status
	void SetDevice(const std::shared_ptr<GraphicsDevice>& device);
	void SetGamma(F32 gamma);
	F32 GetGamma();
	U32 GetShadowCascadeCount();
	U32 GetShadowRes2DResolution();
	F32x3 GetAmbientColor();
	void SetAmbientColor(F32x3 color);
	void SetAlphaCutRef(CommandList cmd, F32 alpha);
	void ResetAlphaCutRef(CommandList cmd);
	void SetScreenSize(U32 width, U32 height);
	U32x2 GetScreenSize();
	XMMATRIX GetScreenProjection();
	Ray GetMainCameraMouseRay(const U32x2& pos);

	// Render Method
	void RenderSceneOpaque(CommandList cmd, CameraComponent& camera, RenderPassType renderPassType);
	void RenderSceneTransparent(CommandList cmd, CameraComponent& camera, RenderPassType renderPassType);
	void RenderImpostor(CommandList cmd, CameraComponent& camera, RenderPassType renderPassType);
	void RenderSky(CommandList cmd);
	void RenderLinearDepth(CommandList cmd, Texture2D& depthBuffer, Texture2D& linearDepthBuffer);
	void RenderSSAO(CommandList cmd, Texture2D& depthBuffer, Texture2D& linearDepthBuffer, Texture2D& aoTexture, F32 aoRange, U32 aoSampleCount);
	void RenderParticles(CommandList cmd, CameraComponent& camera, Texture2D& linearDepthBuffer);

	// shadow
	void RenderShadowmaps(CommandList cmd, CameraComponent& camera, U32 renderLayerMask);
	void RenderDirLightShadowmap(CommandList cmd, LightComponent& light, CameraComponent& camera, U32 renderLayerMask);
	void RenderPointLightShadowmap(CommandList cmd, LightComponent& light, CameraComponent& camera, U32 renderLayerMask);

	// blur
	void GaussianBlur(CommandList cmd, Texture2D& input, Texture2D& temp, Texture2D& output);
	void BilateralBlur(CommandList cmd, Texture2D& input, Texture2D& temp, Texture2D& output, Texture2D& linearDepthBuffer, F32 depthThreshold);
	void UpsampleBilateral(CommandList cmd, Texture2D& input, Texture2D& output, Texture2D& linearDepthBuffer, F32 depthThreshold);

	// tiled light culling
	bool IsTiledCullingDebug();
	U32x2 GetCullingTiledCount();
	void TiledLightCulling(CommandList cmd, Texture2D& depthBuffer);

	// postprocess
	void PostprocessTonemap(CommandList cmd, Texture2D& input, Texture2D& output, F32 exposure);
	void PostprocessFXAA(CommandList cmd, Texture2D& input, Texture2D& output);

	// Binding Method
	void BindCommonResource(CommandList cmd);
	void BindConstanceBuffer(CommandList cmd, SHADERSTAGES stage);
	void BindShadowMaps(CommandList cmd, SHADERSTAGES stage);

	// const buffer function
	void UpdateCameraCB(CommandList cmd, CameraComponent& camera);
	void UpdateFrameCB(CommandList cmd);

	// render path method
	void SetCurrentRenderPath(RenderPath* renderPath);

	// deferred mipmap gen
	void AddDeferredTextureMipGen(Texture2D& texture);

	// render path
	void InitializeRenderPasses();
	void UninitializeRenderPasses();
	void RegisterRenderPass(const StringID& name, std::shared_ptr<RenderPass> renderPath);
	std::shared_ptr<RenderPass> GetRenderPass(const StringID& name);

	// terrain 
	std::shared_ptr<TerrainTree> RegisterTerrain(ECS::Entity entity, U32 width, U32 height, U32 elevation);
	void UnRegisterTerrain(ECS::Entity entity);
	std::shared_ptr<TerrainTree> GetTerrainTree(ECS::Entity entity);

	// debug
	void RenderDebugScene(CommandList cmd, CameraComponent& camera);
	void SetDebugGridSize(const I32x2& gridSize);
	void SetDebugGridOffset(const F32x3& posOffset);

	// shader
	void ReloadShaders();
	ShaderPtr LoadShader(SHADERSTAGES stages, const std::string& path);
	VertexShaderInfo LoadVertexShaderInfo(const std::string& path, VertexLayoutDesc* desc, U32 numElements);
	void RegisterCustomPipelineState(RenderPassType passType, const StringID& name, PipelineStateDesc desc);
	PipelineState* GetPipelineStateByStringID(RenderPassType passType, const StringID& id);

	GraphicsDevice* CreateGraphicsDeviceByType(RenderingDeviceType deviceType, HWND window);
	void ProcessRenderQueue(CommandList cmd, RenderQueue& queue, RenderPassType renderPassType, RenderableType renderableType, U32 instanceReplicator = 1, InstanceHandler* instanceHandler = nullptr);
	void ProcessSkinning(CommandList cmd);
	void ProcessParticles(CommandList cmd, Scene& scene, const std::vector<U32>& culledParticles);

	// ��ǰ֡�Ĳü��������
	struct FrameCullings
	{
		Frustum mFrustum;
		std::vector<U32> mCulledObjects; // ��¼��Ⱦ��object index
		std::vector<U32> mCulledLights;
		std::vector<U32> mCulledParticles;

		void Clear();
	};

	// ÿ֡���õ����Է�����
	LinearAllocator& GetFrameAllocator();
	LinearAllocator& GetRenderAllocator(CommandList cmd);

	// ���ڼ�¼ÿһ֡�Ļ�������
	class RenderFrameData
	{
	public:
		F32x2 mFrameScreenSize = F32x2(0.0f, 0.0f);
		F32x2 mFrameInvScreenSize = F32x2(0.0f, 0.0f);
		U32 mShaderLightArrayCount = 0;
		F32x3 mFrameAmbient = F32x3(0.0f, 0.0f, 0.0f);
		F32 mFrameDeltaTime = 0.0f;
		F32 mFrameTime = 0.0f;;

		void Clear();
	};

	// GPU Query, latencyΪ�ӳٶ�ȡ��������������ͬһ֡������/��ȡͬһ��query
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

		// ����GPU������Ϣʱ����ֱ�ӷ���currentIndex��query
		GPUQuery* GetQueryForGPU()
		{
			mActiveTable[currentIndex] = true;
			return &mQueries[currentIndex];
		}

		// ��CPU�������ݽ��ʱ�������ܻ�ѭ��������һ��Index+1��query��
		// ���ӳ�latency�ζ�ȡ
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

// renderer sub system
class RendererSystem : public SubSystem
{
public:
	void Initialize()
	{
		Renderer::Initialize();
	}
	void Uninitialize()override
	{
		Renderer::Uninitialize();
	}
};

}