#include "renderer.h"
#include "shaderLib.h"
#include "stateManager.h"
#include "renderer\RHI\device.h"
#include "core\gameContext.hpp"
#include "resource\resourceManager.h"

namespace Cjing3D {

Renderer::Renderer(GameContext& gameContext) :
	GameSystem(gameContext),
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

	mIsInitialized = true;
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
	return mGameContext.GetGameSystem<ResourceManager>();
}

}