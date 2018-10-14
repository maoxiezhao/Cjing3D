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
}

}
