#include "deviceD3D11.h"
#include "pipelineD3D11.h"

namespace Cjing3D {

namespace {

	inline DXGI_FORMAT _ConvertFormat(FORMAT format)
	{
		switch (format)
		{
		case Cjing3D::FORMAT_UNKNOWN:
			return DXGI_FORMAT_UNKNOWN;
			break;
		case Cjing3D::FORMAT_R8G8B8A8_UNORM:
			return DXGI_FORMAT_R8G8B8A8_UNORM;
			break;
		default:
			return DXGI_FORMAT_UNKNOWN;
		}
	}

	//inline HRESULT CreateDepthStencilState(DepthStencilState* depthStencilState)
	//{
	//	return S_OK;
	//}
}

GraphicsDeviceD3D11::GraphicsDeviceD3D11(HWND window, bool fullScreen, bool debugLayer):
	GraphicsDevice(),
	mWindow(window),
	mDebugLayer(debugLayer),
	mDevice(),
	mDeviceContext(),
	mSwapChain()
{
	mIsFullScreen = fullScreen;

	mBackBufferFormat = FORMAT_R8G8B8A8_UNORM;

	RECT rect = RECT();
	GetClientRect(window, &rect);
	mScreenSize[0] = rect.right - rect.left;
	mScreenSize[1] = rect.bottom - rect.top;
}

void GraphicsDeviceD3D11::Initialize()
{
	// 初始化device
	InitializeDevice();

	// 初始化swapchain
	mSwapChain = std::make_unique<SwapChainD3D11>(
		*mDevice.Get(),
		GetDeviceContext(GraphicsThread_IMMEDIATE),
		mWindow,
		mScreenSize,
		_ConvertFormat(GetBackBufferFormat())
	);

	// 创建ID3DUserDefinedAnnotation for GraphicsThread_IMMEDIATE
	{
		const auto result = GetDeviceContext(GraphicsThread_IMMEDIATE).QueryInterface(
			__uuidof(mUserDefinedAnnotations[GraphicsThread_IMMEDIATE]),
			reinterpret_cast<void**>(&mUserDefinedAnnotations[GraphicsThread_IMMEDIATE]));
		Debug::ThrowIfFailed(result, "Failed to create ID3DUserDefinedAnnotation: %08X", result);
	}

	// 检查是否支持多线程
	D3D11_FEATURE_DATA_THREADING threadingFeature;
	mDevice->CheckFeatureSupport(D3D11_FEATURE_THREADING, &threadingFeature, sizeof(threadingFeature));
	if (threadingFeature.DriverConcurrentCreates && threadingFeature.DriverCommandLists)
	{
		mIsMultithreadedRendering = true;
		
		for (int i = 0; i < GraphicsThread_COUNT; i++)
		{
			if (i == static_cast<U32>(GraphicsThread_IMMEDIATE)) {
				continue;
			}
			// 创建deferredContext
			ComPtr<ID3D11DeviceContext> deviceContext;
			{
				const auto result = mDevice->CreateDeferredContext(0, &deviceContext);
				Debug::ThrowIfFailed(result, "Failed to create deferred context: %08X", result);
			}

			// 创建ID3DUserDefinedAnnotation
			{
				const auto result = deviceContext->QueryInterface(
					__uuidof(mUserDefinedAnnotations[i]),
					reinterpret_cast<void**>(&mUserDefinedAnnotations[i]));
				Debug::ThrowIfFailed(result, "Failed to create ID3DUserDefinedAnnotation: %08X", result);
			}
			deviceContext.As(&mDeviceContext[i]);
		}
	}

	// 创建视口
	mViewport.mWidth  = static_cast<F32>(mScreenSize[0]);
	mViewport.mHeight = static_cast<F32>(mScreenSize[1]);
	mViewport.mTopLeftX = 0.0f;
	mViewport.mTopLeftY = 0.0f;
	mViewport.mMinDepth = 0.0f;
	mViewport.mMaxDepth = 0.0f;
}

void GraphicsDeviceD3D11::Uninitialize()
{
	mSwapChain.reset();

	for (int i = 0; i < GraphicsThread_COUNT; i++) {
		mDeviceContext[i]->ClearState();
	}
}

void GraphicsDeviceD3D11::PresentBegin()
{

}

void GraphicsDeviceD3D11::PresentEnd()
{
	mSwapChain->Present(mIsVsync);

	PipelineD3D11::OM::BindRTVAndDSV(GetDeviceContext(GraphicsThread_IMMEDIATE), nullptr, nullptr);

	GetDeviceContext(GraphicsThread_IMMEDIATE).ClearState();
}

// 绑定视口
void GraphicsDeviceD3D11::BindViewports(const ViewPort * viewports, U32 numViewports, GraphicsThread threadID)
{
	Debug::ThrowIfFailed(numViewports <= 6);

	D3D11_VIEWPORT d3dViewports[6];
	for (int i = 0; i < numViewports; i++)
	{
		d3dViewports[i].Width = viewports[i].mWidth;
		d3dViewports[i].Height = viewports[i].mHeight;
		d3dViewports[i].MinDepth = viewports[i].mMinDepth;
		d3dViewports[i].MaxDepth = viewports[i].mMaxDepth;
		d3dViewports[i].TopLeftX = viewports[i].mTopLeftX;
		d3dViewports[i].TopLeftY = viewports[i].mTopLeftY;
	}

	PipelineD3D11::RS::BindViewport(GetDeviceContext(threadID), numViewports, d3dViewports);
}

void GraphicsDeviceD3D11::SetViewport(ViewPort viewport)
{
	mViewport = viewport;
}

void GraphicsDeviceD3D11::InitializeDevice()
{
	// 优先D3D_DRIVER_TYPE_HARDWARE
	const static D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};
	U32 numTypes = ARRAYSIZE(driverTypes);

	UINT createDeviceFlags = 0;
	if (mDebugLayer == true) {
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
	}

	const static D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	for (int index = 0; index < numTypes; index++)
	{
		auto driverType = driverTypes[index];

		ComPtr<ID3D11Device> device;
		ComPtr<ID3D11DeviceContext> deviceContext;

		const HRESULT result = D3D11CreateDevice(
			nullptr,
			driverType,
			nullptr,
			createDeviceFlags,
			featureLevels,
			numFeatureLevels,
			D3D11_SDK_VERSION,
			device.GetAddressOf(),
			&mFeatureLevel,
			deviceContext.GetAddressOf());
		
		if (SUCCEEDED(result)) {
			device.As(&mDevice);
			deviceContext.As(&(mDeviceContext[GraphicsThread_IMMEDIATE]));

			break;
		}
	}

	if (mDevice == nullptr || mDeviceContext == nullptr)
	{
		Debug::Error("Failed to initialize d3d device.");
		return;
	}
}


}
