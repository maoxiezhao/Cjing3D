#pragma once

#include "renderer\renderableCommon.h"
#include "renderer\RHI\d3d11\includeD3D11.h"

namespace Cjing3D {

	class SwapChainD3D11
	{
	public:
		SwapChainD3D11(ID3D11Device& device,
			ID3D11DeviceContext& deviceContext,
			HWND window,
			U32x2 resolution,
			DXGI_FORMAT format);

		HWND GetWindow()const {
			return mWindow;
		}

		ID3D11RenderTargetView& GetRenderTargetView()const {
			return *mRenderTargetView.Get();
		}

		void Clear();
		void Present(bool isVsync);

	private:
		void InitAdapterAndOutput();
		void CreateSwapChain(DXGI_FORMAT format);
		void CreateRenderTargetView();

		U32x2 mResolution;
		HWND mWindow;
		ID3D11Device & mDevice;
		ID3D11DeviceContext& mDeviceContext;

		ComPtr<IDXGIAdapter> mAdapter;
		ComPtr<IDXGIOutput> mOutput;
		ComPtr<IDXGISwapChain> mSwapChain;
		ComPtr<ID3D11RenderTargetView> mRenderTargetView;

	};
}