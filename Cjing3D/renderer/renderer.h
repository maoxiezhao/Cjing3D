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

enum RenderableType
{
	RenderableType_Opaque,
	RenderableType_Transparent
};

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

	void RenderSceneOpaque(std::shared_ptr<Camera> camera, RenderingType renderingType);
	void RenderSceneTransparent(std::shared_ptr<Camera> camera, RenderingType renderingType);

	// const buffer function
	void UpdateCameraCB(Camera& camera);

	ShaderInfoState GetShaderInfoState(Material& material);

private:
	void InitializePasses();
	void AccquireActors(std::vector<ActorPtr> actors);
	void UpdateRenderData();
	void ProcessRenderQueue(RenderQueue& queue, RenderingType renderingType, XMMATRIX viewProj);

	// Pass function
	void ForwardRender();

	// 当前帧的裁剪后的数据
	struct FrameCullings
	{
		ActorPtrArray mRenderingActors;
		Frustum mFrustum;

		void Clear();
	};
	std::unordered_map<std::shared_ptr<Camera>, FrameCullings> mFrameCullings;

private:
	bool mIsInitialized;
	bool mIsRendering;
	U32 mFrameNum = 0;

	std::shared_ptr<Camera> mCamera;
	std::unique_ptr<GraphicsDevice> mGraphicsDevice;
	std::unique_ptr<ShaderLib> mShaderLib;
	std::unique_ptr<StateManager> mStateManager;
	std::unique_ptr<Pipeline> mPipeline;
	std::unique_ptr<RenderQueue> mRenderQueue;
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