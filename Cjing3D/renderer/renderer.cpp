#include "renderer.h"
#include "shaderLib.h"

namespace Cjing3D {

Renderer::Renderer() :
	mGraphicsDevice(nullptr),
	mResourceLoader(nullptr),
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

	mResourceLoader = std::make_unique<ResourceLoader>(*this);
	mResourceLoader->Initialize();

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

}