#include "renderer.h"
#include "shaderLib.h"
#include "stateManager.h"
#include "renderer\RHI\device.h"
#include "core\systemContext.hpp"
#include "resource\resourceManager.h"
#include "core\eventSystem.h"
#include "renderer\RHI\deviceD3D11.h"

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
	mCamera(nullptr)
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

	mStateManager = std::make_unique<StateManager>(*mGraphicsDevice);
	mStateManager->SetupStates();

	//mShaderLib = std::make_unique<ShaderLib>(*this);
	//mShaderLib->Initialize();

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

void Renderer::InitializePasses()
{
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

}