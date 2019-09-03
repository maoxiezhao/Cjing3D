#include "device.h"

namespace Cjing3D
{

GraphicsDevice::GraphicsDevice():
	mIsFullScreen(false),
	mIsMultithreadedRendering(false),
	mIsVsync(false)
{
}

void GraphicsDevice::Initialize()
{
}

void GraphicsDevice::Uninitialize()
{
	for (GPUResource* resource : mGPUResource){
		resource->UnRegister();
	}
	mGPUResource.clear();
}

void GraphicsDevice::AddGPUResource(GPUResource * resource)
{
	if (std::find(mGPUResource.begin(), mGPUResource.end(), resource) == mGPUResource.end())
	{
		mGPUResource.push_back(resource);
	}
}

void GraphicsDevice::RemoveGPUResource(GPUResource * resource)
{
	if (std::find(mGPUResource.begin(), mGPUResource.end(), resource) != mGPUResource.end())
	{
		mGPUResource.remove(resource);
	}
}

}
