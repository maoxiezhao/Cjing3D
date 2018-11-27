#include "renderer.h"
#include "shaderLib.h"
#include "stateManager.h"
#include "renderer\RHI\device.h"
#include "core\systemContext.hpp"
#include "resource\resourceManager.h"

namespace Cjing3D {

Renderer::Renderer(SystemContext& gameContext) :
	SubSystem(gameContext),
	mGraphicsDevice(nullptr),
	mShaderLib(nullptr),
	mStateManager(nullptr),
	mIsInitialized(false)
{
}

Renderer::~Renderer()
{
}

void Renderer::Initialize(GraphicsDevice* device)
{
	if (mIsInitialized == true){
		return;
	}

	if (device == nullptr) {
		return;
	}

	mGraphicsDevice = std::unique_ptr<GraphicsDevice>(device);
	mGraphicsDevice->Initialize();

	mStateManager = std::make_unique<StateManager>(*device);
	mStateManager->SetupStates();

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