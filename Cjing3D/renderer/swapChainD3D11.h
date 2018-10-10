#pragma once

#include "renderer\renderableCommon.h"
#include "renderer\includeD3D11.h"

namespace Cjing3D {

	/**
	*	\brief Ωªªª¡¥∂‘œÛ
	*/
	class SwapChainD3D11
	{
	public:
		SwapChainD3D11(ID3D11Device& device,
			ID3D11DeviceContext& deviceContext,
			HWND window);
		~SwapChainD3D11();

		HWND GetWindow()const {
			return mWindow;
		}

		ID3D11RenderTargetView& GetRenderTargetView()const {
			return *mRenderTargetView.Get();
		}

		void Clear();
		void Present();

	private:
		void CreateSwapChain();
		void CreateRenderTargetView();

		HWND mWindow;

		ID3D11Device & mDevice;
		ID3D11DeviceContext& mDeviceContext;

		ComPtr<IDXGISwapChain> mSwapChain;
		ComPtr<ID3D11RenderTargetView> mRenderTargetView;

	};
}