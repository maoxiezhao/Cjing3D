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
class RhiResourceManager;
class ResourceManager;
class CameraComponent;
class LightComponent;
class Scene;
class MaterialComponent;
class PipelineStateManager;
class Renderer2D;
class RenderPass;
class TerrainTree;


// TODO: change to renderer namesapce
// Renderer�����Ϊһ�������ṩ������Ⱦ�����ļ��ϣ�namespace)������ĵ��÷�ʽӦ����renderPath����
class Renderer : public SubSystem
{
public:
	Renderer(SystemContext& gameContext, RenderingDeviceType deviceType, HWND window);
	~Renderer();

	virtual void FixedUpdate();
	virtual void Update(F32 deltaTime);

	void UpdatePerFrameData(F32 deltaTime);
	void RefreshRenderData();

	void Initialize();
	void Uninitialize();
	void Render();
	void Compose();
	void Present();
	
	// getter
	GraphicsDevice& GetDevice();
	ResourceManager& GetResourceManager();
	CameraComponent& GetCamera();
	ShaderLib& GetShaderLib();
	RhiResourceManager& GetStateManager();
	Scene& GetMainScene();
	PipelineStateManager& GetPipelineStateManager();
	Renderer2D& GetRenderer2D();
	RenderPath* GetRenderPath();

	// base status
	void SetGamma(F32 gamma) { mGamma = gamma; }
	F32 GetGamma()const { return mGamma; }
	U32 GetShadowCascadeCount()const;
	U32 GetShadowRes2DResolution()const;
	F32x3 GetAmbientColor()const;
	void SetAmbientColor(F32x3 color);
	void SetAlphaCutRef(F32 alpha);
	void ResetAlphaCutRef() { SetAlphaCutRef(1.0f); }
	void SetScreenSize(U32 width, U32 height);
	U32x2 GetScreenSize()const;

	// Render Method
	void RenderShadowmaps(CameraComponent& camera);
	void RenderSceneOpaque(CameraComponent& camera, RenderPassType renderPassType);
	void RenderSceneTransparent(CameraComponent& camera, RenderPassType renderPassType);
	void RenderImpostor(CameraComponent& camera, RenderPassType renderPassType);
	void RenderSky();

	bool IsTiledCullingDebug()const;
	U32x2 GetCullingTiledCount()const;
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

	// shader
	ShaderPtr LoadShader(SHADERSTAGES stages, const std::string& path);
	VertexShaderInfo LoadVertexShaderInfo(const std::string& path, VertexLayoutDesc* desc, U32 numElements);

private:
	void InitializeRenderPasses();
	void UninitializeRenderPasses();

	GraphicsDevice* CreateGraphicsDeviceByType(RenderingDeviceType deviceType, HWND window);
	void ProcessRenderQueue(RenderQueue& queue, RenderPassType renderPassType, RenderableType renderableType, U32 instanceReplicator = 1, InstanceHandler* instanceHandler = nullptr);
	void RenderDirLightShadowmap(LightComponent& light, CameraComponent& camera);
	void RenderPointLightShadowmap(LightComponent& light, CameraComponent& camera);
	void ProcessSkinning();

	// ��ǰ֡�Ĳü��������
	struct FrameCullings
	{
		Frustum mFrustum;
		std::vector<U32> mCulledObjects; // ��¼��Ⱦ��object index
		std::vector<U32> mCulledLights;

		void Clear();
	};
	std::unordered_map<const CameraComponent*, FrameCullings> mFrameCullings;

public:
	// ÿ֡���õ����Է�����
	enum FrameAllocatorType
	{
		FrameAllocatorType_Update = 0,
		FrameAllocatorType_Render,
		FrameAllocatorType_Count
	};
	LinearAllocator& GetFrameAllocator(FrameAllocatorType type);

	// ���ڼ�¼ÿһ֡�Ļ�������
	class RenderFrameData
	{
	public:
		F32x2 mFrameScreenSize;
		F32x2 mFrameInvScreenSize;
		U32 mShaderLightArrayCount;
		F32x3 mFrameAmbient;

		void Clear();
	};
	RenderFrameData mFrameData;

	// GPU Query, latencyΪ�ӳٶ�ȡ��������������ͬһ֡������/��ȡͬһ��query
	template<I32 latency>
	class GPUQueryHandler
	{
	public:
		GPUQueryHandler() {};
		~GPUQueryHandler() { Uninitialize(); }

		void Initialize(Renderer& renderer, GPUQueryDesc& desc)
		{
			if (IsInitialized()) {
				return;
			}

			for (int i = 0; i < latency; i++)
			{
				renderer.GetDevice().CreateQuery(desc, mQueries[i]);
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

private:
	bool mIsInitialized;
	bool mIsRendering;
	U32 mFrameNum = 0;
	U32x2 mScreenSize;
	F32 mGamma = 2.2f;
	CommonCB mCommonCB;
	std::vector<int> mPendingUpdateMaterials;
	LinearAllocator mFrameAllocator[FrameAllocatorType_Count];

	// base member
	std::unique_ptr<GraphicsDevice> mGraphicsDevice;
	std::unique_ptr<ShaderLib> mShaderLib;
	std::unique_ptr<RhiResourceManager> mRhiResourceManager;
	std::unique_ptr<DeferredMIPGenerator> mDeferredMIPGenerator;
	std::unique_ptr<PipelineStateManager> mPipelineStateManager;
	std::unique_ptr<Renderer2D> mRenderer2D;
	std::unique_ptr<RenderPath> mCurrentRenderPath;

	// render pass
	std::map<StringID, std::shared_ptr<RenderPass>> mRenderPassMap;
};

}