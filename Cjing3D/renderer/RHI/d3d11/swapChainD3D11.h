#pragma once

#include "renderer\renderableCommon.h"
#include "renderer\RHI\d3d11\includeD3D11.h"

namespace Cjing3D {

	class GameWindow;

	class SwapChainD3D11
	{
	public:
		SwapChainD3D11(ID3D11Device& device,
			ID3D11DeviceContext& deviceContext,
			const GameWindow& gameWindow,
			U32x2 resolution,
			DXGI_FORMAT format);

		ID3D11RenderTargetView& GetRenderTargetView()const {
			return *mRenderTargetView.Get();
		}

		void Clear();
		void Present(bool isVsync);
		void Resize(const U32x2& size);

	private:
		void InitAdapterAndOutput();
		void CreateSwapChain(const GameWindow& gameWindow, DXGI_FORMAT format);
		void CreateRenderTargetView();

		U32x2 mResolution;
		ID3D11Device & mDevice;
		ID3D11DeviceContext& mDeviceContext;
		U32 mBackBufferCount = 2;
		DXGI_FORMAT mBackBufferFormat;

		ComPtr<IDXGIAdapter> mAdapter;
		ComPtr<IDXGIOutput> mOutput;
		ComPtr<IDXGISwapChain> mSwapChain;
		ComPtr<ID3D11RenderTargetView> mRenderTargetView;

	};
}