#pragma once

#include "renderer\renderableCommon.h"
#include "renderer\RHI\rhiPipeline.h"
#include "renderer\pass\forwardPass.h"
#include "renderer\rendererUtils.h"
#include "core\subSystem.hpp"
#include "world\actor.h"
#include "utils\allocator.h"

#include <unordered_map>

namespace Cjing3D
{

class GraphicsDevice;
class ShaderLib;
class StateManager;
class ResourceManager;
class Camera;
class Scene;

struct MaterialComponent;

class Renderer : public SubSystem
{
public:
	Renderer(SystemContext& gameContext, RenderingDeviceType deviceType, HWND window);
	~Renderer();

	void Initialize();
	void Update();
	void Uninitialize();
	void Render();
	void Present();
	
	GraphicsDevice& GetDevice();
	ResourceManager& GetResourceManager();
	Pipeline& GetPipeline();
	DeferredMIPGenerator& GetDeferredMIPGenerator();
	std::shared_ptr<Camera> GetCamera();

	void RenderSceneOpaque(std::shared_ptr<Camera> camera, ShaderType shaderType);
	void RenderSceneTransparent(std::shared_ptr<Camera> camera, ShaderType renderingType);

	// const buffer function
	void UpdateCameraCB(Camera& camera);

	ShaderInfoState GetShaderInfoState(MaterialComponent& material);
	Scene& GetMainScene();

private:
	void InitializePasses();
	void AccquireActors(std::vector<ActorPtr> actors);
	void UpdateRenderData();
	void ProcessRenderQueue(RenderQueue& queue, ShaderType shaderType, RenderableType renderableType);
	void ProcessRenderQueue(RenderQueue& queue, ShaderType renderingType, XMMATRIX viewProj);
	void UpdateScene();

	// Pass function
	void ForwardRender();

	// 当前帧的裁剪后的数据
	struct FrameCullings
	{
		ActorPtrArray mRenderingActors;
		Frustum mFrustum;

		// 记录渲染的object index
		std::vector<U32> mRenderingObjects;

		void Clear();
	};
	std::unordered_map<std::shared_ptr<Camera>, FrameCullings> mFrameCullings;

private:
	bool mIsInitialized;
	bool mIsRendering;
	U32 mFrameNum = 0;
	U32x2 mScreenSize;

	std::shared_ptr<Camera> mCamera;
	std::unique_ptr<GraphicsDevice> mGraphicsDevice;
	std::unique_ptr<ShaderLib> mShaderLib;
	std::unique_ptr<StateManager> mStateManager;
	std::unique_ptr<Pipeline> mPipeline;
	std::unique_ptr<DeferredMIPGenerator> mDeferredMIPGenerator;
	std::unique_ptr<LinearAllocator> mFrameAllocator;

	/** rendering pass */
	std::unique_ptr<ForwardPass> mForwardPass;

	// temp define //////////////////////////
	float mNearPlane;
	float mFarPlane;
	XMMATRIX mView;
	XMMATRIX mProjection;
	XMMATRIX mViewProjection;

	void PassGBuffer();

};

}