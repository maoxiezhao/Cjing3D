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
class StateManager;
class ResourceManager;
class CameraComponent;
class LightComponent;
class BufferManager;
class Scene;
class MaterialComponent;
class PipelineStateManager;
class Renderer2D;
class RenderPass;
class TerrainTree;

// 用于记录每一帧的基础数据
class RenderFrameData
{
public:
	F32x2 mFrameScreenSize;
	U32 mShaderLightArrayCount;
	F32x3 mFrameAmbient;

	void Clear();
};

// TODO: change to renderer namesapce
// Renderer被设计为一个仅仅提供各种渲染方法的集合（namespace)，具体的调用方式应该由renderPath决定
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
	U32x2 GetScreenSize();
	GraphicsDevice& GetDevice();
	ResourceManager& GetResourceManager();
	CameraComponent& GetCamera();
	ShaderLib& GetShaderLib();
	StateManager& GetStateManager();
	BufferManager& GetBufferManager();
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

	// Render Method
	void RenderShadowmaps(CameraComponent& camera);
	void RenderSceneOpaque(CameraComponent& camera, RenderPassType renderPassType);
	void RenderSceneTransparent(CameraComponent& camera, RenderPassType renderPassType);
	void RenderImpostor(CameraComponent& camera, RenderPassType renderPassType);
	void RenderSky();

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

	// 当前帧的裁剪后的数据
	struct FrameCullings
	{
		Frustum mFrustum;
		std::vector<U32> mCulledObjects; // 记录渲染的object index
		std::vector<U32> mCulledLights;

		void Clear();
	};
	std::unordered_map<const CameraComponent*, FrameCullings> mFrameCullings;

public:
	// 每帧所用的线性分配器
	enum FrameAllocatorType
	{
		FrameAllocatorType_Update = 0,
		FrameAllocatorType_Render,
		FrameAllocatorType_Count
	};
	LinearAllocator& GetFrameAllocator(FrameAllocatorType type);

private:
	bool mIsInitialized;
	bool mIsRendering;
	U32 mFrameNum = 0;
	U32x2 mScreenSize;
	F32 mGamma = 2.2f;
	RenderFrameData mFrameData;
	CommonCB mCommonCB;
	std::vector<int> mPendingUpdateMaterials;
	LinearAllocator mFrameAllocator[FrameAllocatorType_Count];

	// base member
	std::unique_ptr<GraphicsDevice> mGraphicsDevice;
	std::unique_ptr<ShaderLib> mShaderLib;
	std::unique_ptr<StateManager> mStateManager;
	std::unique_ptr<BufferManager> mBufferManager;
	std::unique_ptr<DeferredMIPGenerator> mDeferredMIPGenerator;
	std::unique_ptr<PipelineStateManager> mPipelineStateManager;
	std::unique_ptr<Renderer2D> mRenderer2D;
	std::unique_ptr<RenderPath> mCurrentRenderPath;

	// render pass
	std::map<StringID, std::shared_ptr<RenderPass>> mRenderPassMap;
};

}