#pragma once

#include "renderer\renderableCommon.h"
#include "renderer\paths\renderPath_forward.h"
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
class Scene;
class MaterialComponent;
class PipelineStateInfoManager;

class Renderer : public SubSystem
{
public:
	Renderer(SystemContext& gameContext, RenderingDeviceType deviceType, HWND window);
	~Renderer();

	virtual void Update(F32 deltaTime);

	void Initialize();
	void Uninitialize();
	void UpdateRenderFrameData(F32 deltaTime);
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

	// Render Method
	void RenderSceneOpaque(std::shared_ptr<CameraComponent> camera, ShaderType shaderType);
	void RenderSceneTransparent(std::shared_ptr<CameraComponent> camera, ShaderType renderingType);
	void RenderPostprocess(Texture2D& input, Texture2D& output);

	// Binding Method
	void BindCommonResource();

	// const buffer function
	void UpdateCameraCB(CameraComponent& camera);

private:
	void InitializeRenderPaths();
	void ProcessRenderQueue(RenderQueue& queue, ShaderType shaderType, RenderableType renderableType);
	void BindConstanceBuffer(SHADERSTAGES stage);

	// Pass function
	void ForwardRender();

	// 当前帧的裁剪后的数据
	struct FrameCullings
	{
		Frustum mFrustum;
		std::vector<U32> mRenderingObjects; // 记录渲染的object index

		void Clear();
	};
	std::unordered_map<std::shared_ptr<CameraComponent>, FrameCullings> mFrameCullings;

private:
	bool mIsInitialized;
	bool mIsRendering;
	U32 mFrameNum = 0;
	U32x2 mScreenSize;

	std::shared_ptr<CameraComponent> mCamera;
	std::unique_ptr<GraphicsDevice> mGraphicsDevice;
	std::unique_ptr<ShaderLib> mShaderLib;
	std::unique_ptr<StateManager> mStateManager;
	std::unique_ptr<DeferredMIPGenerator> mDeferredMIPGenerator;
	std::unique_ptr<LinearAllocator> mFrameAllocator;
	std::unique_ptr<PipelineStateInfoManager> mPipelineStateInfoManager;

	std::vector<int> mPendingUpdateMaterials;

	std::unique_ptr<RenderPathForward> mRenderPathForward;
};

}