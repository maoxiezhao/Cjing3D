#include "deviceD3D11.h"

namespace Cjing3D {

GraphicsDeviceD3D11::GraphicsDeviceD3D11(HWND window, bool fullScreen):
	GraphicsDevice(),
	mDevice(),
	mDeviceContext()
{
	mIsFullScreen = fullScreen;
}

void GraphicsDeviceD3D11::Initialize()
{
}

void GraphicsDeviceD3D11::Uninitialize()
{
}

}
