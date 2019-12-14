#pragma once

#include "renderer\renderableCommon.h"
#include "renderer\paths\renderPath.h"
#include "renderer\rendererUtils.h"
#include "core\subSystem.hpp"
#include "utils\allocator.h"

#include <unordered_map>

namespace Cjing3D
{

class GraphicsDevice;
class ShaderLib;
class StateManager;
class ResourceManager;
class CameraComponent;
class BufferManager;
class Scene;
class MaterialComponent;
class PipelineStateInfoManager;
class Renderer2D;

// 用于记录每一帧的基础数据
class RenderFrameData
{
public:
	F32x2 mFrameScreenSize;
	U32 mShaderLightArrayCount;
	F32x3 mFrameAmbient;

	void Clear();
};

class Renderer : public SubSystem
{
public:
	Renderer(SystemContext& gameContext, RenderingDeviceType deviceType, HWND window);
	~Renderer();

	virtual void Update(F32 deltaTime);

	void UpdatFrameData(F32 deltaTime);
	void UpdateRenderData();

	void Initialize();
	void Uninitialize();
	void Render();
	void Compose();
	void Present();
	
	GraphicsDevice& GetDevice();
	ResourceManager& GetResourceManager();
	DeferredMIPGenerator& GetDeferredMIPGenerator();
	std::shared_ptr<CameraComponent> GetCamera();
	ShaderLib& GetShaderLib();
	StateManager& GetStateManager();
	Scene& GetMainScene();
	PipelineStateInfoManager& GetPipelineStateInfoManager();
	Renderer2D& GetRenderer2D();

	// Render Method
	void RenderSceneOpaque(std::shared_ptr<CameraComponent> camera, ShaderType shaderType);
	void RenderSceneTransparent(std::shared_ptr<CameraComponent> camera, ShaderType renderingType);
	void RenderPostprocess(Texture2D& input, Texture2D& output);

	// Binding Method
	void BindCommonResource();

	// const buffer function
	void UpdateCameraCB(CameraComponent& camera);
	void UpdateFrameCB();

	// render path method
	void SetCurrentRenderPath(RenderPath* renderPath);

private:
	void ProcessRenderQueue(RenderQueue& queue, ShaderType shaderType, RenderableType renderableType);
	void BindConstanceBuffer(SHADERSTAGES stage);

	// 当前帧的裁剪后的数据
	struct FrameCullings
	{
		Frustum mFrustum;
		std::vector<U32> mCulledObjects; // 记录渲染的object index
		std::vector<U32> mCulledLights;

		void Clear();
	};
	std::unordered_map<std::shared_ptr<CameraComponent>, FrameCullings> mFrameCullings;

private:
	bool mIsInitialized;
	bool mIsRendering;
	U32 mFrameNum = 0;
	U32x2 mScreenSize;

	RenderFrameData mFrameData;

	std::vector<int> mPendingUpdateMaterials;

	std::shared_ptr<CameraComponent> mCamera;
	std::unique_ptr<GraphicsDevice> mGraphicsDevice;
	std::unique_ptr<ShaderLib> mShaderLib;
	std::unique_ptr<StateManager> mStateManager;
	std::unique_ptr<BufferManager> mBufferManager;
	std::unique_ptr<DeferredMIPGenerator> mDeferredMIPGenerator;
	std::unique_ptr<LinearAllocator> mFrameAllocator;
	std::unique_ptr<PipelineStateInfoManager> mPipelineStateInfoManager;
	std::unique_ptr<Renderer2D> mRenderer2D;

	std::unique_ptr<RenderPath> mCurrentRenderPath;
};

}