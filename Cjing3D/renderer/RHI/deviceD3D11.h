#pragma once

#include "renderer\includeD3D11.h"
#include "device.h"

namespace Cjing3D
{
	class GraphicsDeviceD3D11 : public GraphicsDevice
	{
	public:
		GraphicsDeviceD3D11(HWND window, bool fullScreen);

		virtual void Initialize();
		virtual void Uninitialize();

	private:
		ComPtr<ID3D11Device> mDevice;
		ComPtr<ID3D11DeviceContext> mDeviceContext;
	};
}