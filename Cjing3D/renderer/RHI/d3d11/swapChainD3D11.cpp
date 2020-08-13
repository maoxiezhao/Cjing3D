#include "swapChainD3D11.h"

namespace Cjing3D {

	SwapChainD3D11::SwapChainD3D11(
		ID3D11Device & device, 
		ID3D11DeviceContext & deviceContext, 
		HWND window, 
		U32x2 resolution,
		DXGI_FORMAT format):
		mResolution(resolution),
		mDevice(device),
		mDeviceContext(deviceContext),
		mWindow(window),
		mSwapChain(),
		mRenderTargetView()
	{
		InitAdapterAndOutput();
		CreateSwapChain(format);
		CreateRenderTargetView();
	}

	void SwapChainD3D11::Clear()
	{
		static const float rgba[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		mDeviceContext.ClearRenderTargetView(mRenderTargetView.Get(), rgba);
	}

	void SwapChainD3D11::Present(bool isVsync)
	{
		mSwapChain->Present(isVsync, 0);
	}

	void SwapChainD3D11::InitAdapterAndOutput()
	{
		// create factory
		ComPtr<IDXGIFactory> factory;
		{
			const HRESULT result = CreateDXGIFactory(
				__uuidof(IDXGIFactory), (void**)factory.GetAddressOf());
			Debug::ThrowIfFailed(result, "Failed to create factory.");
		}

		ComPtr<IDXGIAdapter> selectedAdapter;
		ComPtr<IDXGIOutput> selectedOutput;

		// 遍历所有显示适配器，选择显存最大的适配器（和Output）
		SIZE_T maxVarm = 0;
		ComPtr<IDXGIAdapter> iteratedAdapter;
		for (UINT i = 0; factory->EnumAdapters(
			i, iteratedAdapter.ReleaseAndGetAddressOf()) != DXGI_ERROR_NOT_FOUND; ++i)
		{
			ComPtr<IDXGIOutput> iteratedOutput;
			if (FAILED(iteratedAdapter->EnumOutputs(0, iteratedOutput.ReleaseAndGetAddressOf())))
				break;

			// 获取记录显卡信息
			DXGI_ADAPTER_DESC adapterDesc;
			{
				const HRESULT result = iteratedAdapter->GetDesc(&adapterDesc);
				Debug::ThrowIfFailed(result, "Failed to get adapter desc.");
			}

			// 选择显存更大的适配器
			const auto vram = adapterDesc.DedicatedVideoMemory;
			if (vram <= maxVarm)
				continue;

			selectedAdapter = iteratedAdapter;
			selectedOutput = iteratedOutput;
			maxVarm = vram;
		}

		{
			const HRESULT result = selectedAdapter.As(&mAdapter);
			Debug::ThrowIfFailed(result, "Failed to create adapter: %08X.", result);
		}
		{
			const HRESULT result = selectedOutput.As(&mOutput);
			Debug::ThrowIfFailed(result, "Failed to create output: %08X.", result);
		}
	}

	void SwapChainD3D11::CreateSwapChain(DXGI_FORMAT format)
	{
		ComPtr< IDXGIFactory2> factory;
		{
			const HRESULT result = mAdapter->GetParent(
				__uuidof(IDXGIFactory2),
				(void**)(factory.GetAddressOf()));
			Debug::ThrowIfFailed(result, "Failed to create IDXGIFactory2: %08X", result);
		}

		/* 关闭一些窗体全屏特性
		* DXGI_MWA_NO_WINDOW_CHANGES: 不响应窗口改变时切换模式
		* DXGI_MWA_NO_ALT_ENTER: 不想要 ALT-ENTER 键改变模式
		* DXGI_MWA_NO_PRINT_SCREEN： 不响应系统录屏？
		*/
		factory->MakeWindowAssociation(mWindow,
			DXGI_MWA_NO_WINDOW_CHANGES |
			DXGI_MWA_NO_ALT_ENTER |
			DXGI_MWA_NO_PRINT_SCREEN);

		// 描述交换链
		DXGI_SWAP_CHAIN_DESC1 desc = {};
		desc.Width = mResolution[0];
		desc.Height = mResolution[1];
		desc.Format = format;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.BufferCount = 2;
		desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;	// 提交后抛弃后备缓存数据
		desc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
		desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		desc.SampleDesc.Count = 1;						    // 关闭多重采样

		// 描述全屏交换链
		DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullDesc = {};
		fullDesc.RefreshRate = { 60, 1 };
		fullDesc.Windowed = true;

		// 创建交换链
		ComPtr<IDXGISwapChain1> swapChain1;
		{
			const HRESULT result = factory->CreateSwapChainForHwnd(
				&mDevice,
				mWindow,
				&desc,
				&fullDesc,
				nullptr,
				swapChain1.ReleaseAndGetAddressOf());
			Debug::ThrowIfFailed(result, "Failed to create SwapChain: %08X", result);

			if (FAILED(swapChain1.As(&mSwapChain)))
				Debug::ThrowIfFailed(result, "Failed to create SwapChain");
		}

		// 默认设置为FALSE
		mSwapChain->SetFullscreenState(FALSE, nullptr);
	}

	void SwapChainD3D11::CreateRenderTargetView()
	{
		ComPtr<ID3D11Texture2D> backBuffer;
		{
			const HRESULT result = mSwapChain->GetBuffer(
				0, __uuidof(ID3D11Texture2D), (void**)(backBuffer.GetAddressOf()));
			Debug::ThrowIfFailed(result, "Failed to create back buffer texture: %08X", result);
		}

		{
			const HRESULT result = mDevice.CreateRenderTargetView(
				backBuffer.Get(),
				nullptr,
				mRenderTargetView.ReleaseAndGetAddressOf());
			Debug::ThrowIfFailed(result, "Failed to create render target view: %08X", result);
		}
	}
}

