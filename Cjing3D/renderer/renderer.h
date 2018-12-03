#pragma once

#include "renderer\renderableCommon.h"
#include "core\subSystem.hpp"
#include "world\actor.h"
#include "renderer\RHI\rhiPipeline.h"
#include "renderer\pass\forwardPass.h"

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

	/** Rendering setting */

private:
	void InitializePasses();
	void AccquireActors(std::vector<ActorPtr> actors);

private:
	bool mIsInitialized;
	bool mIsRendering;
	U32 mFrameNum = 0;

	std::shared_ptr<Camera> mCamera;

	std::unique_ptr<GraphicsDevice> mGraphicsDevice;
	std::unique_ptr<ShaderLib> mShaderLib;
	std::unique_ptr<StateManager> mStateManager;
	std::unique_ptr<Pipeline> mPipeline;

	std::unordered_map<RenderableType, ActorPtrArray> mRenderingActors;

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