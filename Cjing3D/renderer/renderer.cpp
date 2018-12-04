#include "renderer.h"
#include "shaderLib.h"
#include "stateManager.h"
#include "renderer\RHI\device.h"
#include "core\systemContext.hpp"
#include "core\eventSystem.h"
#include "helper\profiler.h"
#include "renderer\RHI\deviceD3D11.h"
#include "world\component\camera.h"
#include "resource\resourceManager.h"

namespace Cjing3D {

namespace {
	GraphicsDevice* CreateGraphicsDeviceByType(RenderingDeviceType deviceType, HWND window)
	{
		GraphicsDevice* graphicsDevice = nullptr;
		switch (deviceType)
		{
		case RenderingDeviceType_D3D11:
			graphicsDevice = new GraphicsDeviceD3D11(window, false, true);
			break;
		}
		return graphicsDevice;
	}
}

Renderer::Renderer(SystemContext& gameContext, RenderingDeviceType deviceType, HWND window) :
	SubSystem(gameContext),
	mGraphicsDevice(nullptr),
	mShaderLib(nullptr),
	mStateManager(nullptr),
	mIsInitialized(false),
	mIsRendering(false),
	mCamera(nullptr),
	mFrameNum(0)
{
	mGraphicsDevice = std::unique_ptr<GraphicsDevice>(CreateGraphicsDeviceByType(deviceType, window));
}

Renderer::~Renderer()
{
}

void Renderer::Initialize()
{
	if (mIsInitialized == true){
		return;
	}

	mGraphicsDevice->Initialize();

	// initialize states
	mStateManager = std::make_unique<StateManager>(*mGraphicsDevice);
	mStateManager->SetupStates();

	// initialize shader
	mShaderLib = std::make_unique<ShaderLib>(*this);
	mShaderLib->Initialize();

	InitializePasses();

	mIsInitialized = true;
}

void Renderer::Update()
{
}

void Renderer::Uninitialize()
{
	if (mIsInitialized == false) {
		return;
	}

	mGraphicsDevice->Uninitialize();

	mIsInitialized = false;
}

void Renderer::Render()
{
	if (mCamera == nullptr || mRenderingActors.empty()) {
		mIsRendering = false;
		return;
	}

	return;

	// global data
	{
		mNearPlane = mCamera->GetNearPlane();
		mFrameNum = mCamera->GetFarPlane();
		mView = mCamera->GetViewMatrix();
		mProjection = mCamera->GetProjectionMatrix();
		mViewProjection = mCamera->GetViewProjectionMatrix();
	}

	mIsRendering = true;
	mFrameNum++;

	//PassGBuffer();

	mIsRendering = false;
}

void Renderer::Present()
{
	mGraphicsDevice->PresentBegin();
	mGraphicsDevice->PresentEnd();
}

GraphicsDevice & Renderer::GetDevice()
{
	return *mGraphicsDevice;
}

ResourceManager & Renderer::GetResourceManager()
{
	return mGameContext.GetSubSystem<ResourceManager>();
}

Pipeline & Renderer::GetPipeline()
{
	return *mPipeline;
}

void Renderer::InitializePasses()
{
	mForwardPass = std::make_unique<ForwardPass>(mGameContext);
}

/**
*	\brief 接受参与渲染的actor， 在world渲染时传入actors
*/
void Renderer::AccquireActors(std::vector<ActorPtr> actors)
{
	mRenderingActors.clear();

	for (const auto& actor : actors)
	{
		if (actor == nullptr) {
			continue;
		}

		auto renderable = actor->GetComponents<Renderable>();
	}

	// sort renderging actors
	if (mRenderingActors.size() > 0)
	{

	}
}

// 以延迟渲染方式绘制不透明物体
void Renderer::PassGBuffer()
{
	if (mRenderingActors[RenderableType::RenderableType_Opaque].empty()) {
		return;
	}

	auto& profiler = Profiler::GetInstance();
	profiler.BeginBlock("GBuffer_Pass");

	auto gBufferShader = mShaderLib->GetVertexShader(VertexShaderType_Transform);

	mPipeline->SetPrimitiveTopology(PRIMITIVE_TOPOLOGY::TRIANGLELIST);
	mPipeline->SetFillMode(FILL_SOLID);
	mPipeline->SetVertexShader(gBufferShader);

	for (auto actor : mRenderingActors[RenderableType::RenderableType_Opaque])
	{

	}

	profiler.EndBlock();
}

}