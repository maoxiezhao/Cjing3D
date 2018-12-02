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

	inline D3D11_DEPTH_WRITE_MASK _ConvertDepthWriteMask(DepthWriteMask mask)
	{
		switch (mask)
		{
		case Cjing3D::DEPTH_WRITE_MASK_ZERO:
			return D3D11_DEPTH_WRITE_MASK_ZERO;
		case Cjing3D::DEPTH_WRITE_MASK_ALL:
			return D3D11_DEPTH_WRITE_MASK_ALL;
		default:
			break;
		}
		return D3D11_DEPTH_WRITE_MASK_ZERO;
	}

	inline D3D11_COMPARISON_FUNC _ConvertComparisonFunc(ComparisonFunc func)
	{
		switch (func)
		{
		case Cjing3D::COMPARISON_NEVER:
			return D3D11_COMPARISON_NEVER;
		case Cjing3D::COMPARISON_LESS:
			return D3D11_COMPARISON_LESS;
		case Cjing3D::COMPARISON_EQUAL:
			return D3D11_COMPARISON_EQUAL;
		case Cjing3D::COMPARISON_LESS_EQUAL:
			return D3D11_COMPARISON_LESS_EQUAL;
		case Cjing3D::COMPARISON_GREATER:
			return D3D11_COMPARISON_GREATER;
		case Cjing3D::COMPARISON_NOT_EQUAL:
			return D3D11_COMPARISON_NOT_EQUAL;
		case Cjing3D::COMPARISON_GREATER_EQUAL:
			return D3D11_COMPARISON_GREATER_EQUAL;
		case Cjing3D::COMPARISON_ALWAYS:
			return D3D11_COMPARISON_ALWAYS;
		default:
			break;
		}
		return D3D11_COMPARISON_NEVER;
	}

	inline D3D11_STENCIL_OP _ConvertStencilOp(StencilOp op)
	{
		switch (op)
		{
		case Cjing3D::STENCIL_OP_KEEP:
			return D3D11_STENCIL_OP_KEEP;
		case Cjing3D::STENCIL_OP_ZERO:
			return D3D11_STENCIL_OP_ZERO;
		case Cjing3D::STENCIL_OP_REPLACE:
			return D3D11_STENCIL_OP_REPLACE;
		case Cjing3D::STENCIL_OP_INCR_SAT:
			return D3D11_STENCIL_OP_INCR_SAT;
		case Cjing3D::STENCIL_OP_DECR_SAT:
			return D3D11_STENCIL_OP_DECR_SAT;
		case Cjing3D::STENCIL_OP_INVERT:
			return D3D11_STENCIL_OP_INVERT;
		case Cjing3D::STENCIL_OP_INCR:
			return D3D11_STENCIL_OP_INCR;
		case Cjing3D::STENCIL_OP_DECR:
			return D3D11_STENCIL_OP_DECR;
		default:
			break;
		}
		return D3D11_STENCIL_OP_KEEP;
	}

	inline D3D11_BLEND _ConvertBlend(Blend blend)
	{
		switch (blend)
		{
		case Cjing3D::BLEND_ZERO:
			return D3D11_BLEND_ZERO;
		case Cjing3D::BLEND_ONE:
			return D3D11_BLEND_ONE;
		case Cjing3D::BLEND_SRC_COLOR:
			return D3D11_BLEND_SRC_COLOR;
		case Cjing3D::BLEND_INV_SRC_COLOR:
			return D3D11_BLEND_INV_SRC1_COLOR;
		case Cjing3D::BLEND_SRC_ALPHA:
			return D3D11_BLEND_SRC_ALPHA;
		case Cjing3D::BLEND_INV_SRC_ALPHA:
			return D3D11_BLEND_INV_SRC_ALPHA;
		case Cjing3D::BLEND_DEST_ALPHA:
			return D3D11_BLEND_DEST_ALPHA;
		case Cjing3D::BLEND_INV_DEST_ALPHA:
			return D3D11_BLEND_INV_DEST_ALPHA;
		case Cjing3D::BLEND_DEST_COLOR:
			return D3D11_BLEND_DEST_COLOR;
		case Cjing3D::BLEND_INV_DEST_COLOR:
			return D3D11_BLEND_INV_DEST_COLOR;
		case Cjing3D::BLEND_SRC_ALPHA_SAT:
			return D3D11_BLEND_SRC_ALPHA_SAT;
		case Cjing3D::BLEND_BLEND_FACTOR:
			return D3D11_BLEND_BLEND_FACTOR;
		case Cjing3D::BLEND_INV_BLEND_FACTOR:
			return D3D11_BLEND_INV_BLEND_FACTOR;
		case Cjing3D::BLEND_SRC1_COLOR:
			return D3D11_BLEND_SRC1_COLOR;
		case Cjing3D::BLEND_INV_SRC1_COLOR:
			return D3D11_BLEND_INV_SRC1_COLOR;
		case Cjing3D::BLEND_SRC1_ALPHA:
			return D3D11_BLEND_SRC1_ALPHA;
		case Cjing3D::BLEND_INV_SRC1_ALPHA:
			return D3D11_BLEND_INV_SRC1_ALPHA;
		default:
			break;
		}
		return D3D11_BLEND_ZERO;
	}

	inline D3D11_BLEND_OP _ConvertBlendOp(BlendOp op)
	{
		switch (op)
		{
		case Cjing3D::BLEND_OP_ADD:
			return D3D11_BLEND_OP_ADD;
		case Cjing3D::BLEND_OP_SUBTRACT:
			return D3D11_BLEND_OP_SUBTRACT;
		case Cjing3D::BLEND_OP_REV_SUBTRACT:
			return D3D11_BLEND_OP_REV_SUBTRACT;
		case Cjing3D::BLEND_OP_MIN:
			return D3D11_BLEND_OP_MIN;
		case Cjing3D::BLEND_OP_MAX:
			return D3D11_BLEND_OP_MAX;
		default:
			break;
		}
		return D3D11_BLEND_OP_ADD;
	}

	inline U32 _ParseColorWriteMask(U32 value)
	{
		U32 flag;
		if (value == ColorWriteEnable::COLOR_WRITE_ENABLE_ALL) {
			return D3D11_COLOR_WRITE_ENABLE_ALL;
		}

		if (value & ColorWriteEnable::COLOR_WRITE_ENABLE_RED) {
			flag |= D3D11_COLOR_WRITE_ENABLE_RED;
		}
		if (value & ColorWriteEnable::COLOR_WRITE_ENABLE_GREEN) {
			flag |= D3D11_COLOR_WRITE_ENABLE_GREEN;
		}
		if (value & ColorWriteEnable::COLOR_WRITE_ENABLE_BLUE) {
			flag |= D3D11_COLOR_WRITE_ENABLE_BLUE;
		}
		if (value & ColorWriteEnable::COLOR_WRITE_ENABLE_ALPHA) {
			flag |= D3D11_COLOR_WRITE_ENABLE_ALPHA;
		}
		return flag;
	}

	inline D3D11_FILL_MODE _ConvertFillMode(FillMode mode)
	{
		switch (mode)
		{
		case Cjing3D::FILL_WIREFRAME:
			return D3D11_FILL_WIREFRAME;
		case Cjing3D::FILL_SOLID:
			return D3D11_FILL_SOLID;
		default:
			break;
		}
		return D3D11_FILL_SOLID;
	}

	inline D3D11_CULL_MODE _ConvertCullMode(CullMode mode)
	{
		switch (mode)
		{
		case Cjing3D::CULL_NONE:
			return D3D11_CULL_NONE;
		case Cjing3D::CULL_FRONT:
			return D3D11_CULL_FRONT;
		case Cjing3D::CULL_BACK:
			return D3D11_CULL_BACK;
		default:
			break;
		}
		return D3D11_CULL_NONE;
	}

	inline D3D11_INPUT_CLASSIFICATION _ConvertInputClassification(InputClassification inputClassification)
	{
		switch (inputClassification)
		{
		case Cjing3D::INPUT_PER_VERTEX_DATA:
			return D3D11_INPUT_PER_VERTEX_DATA;
		case Cjing3D::INPUT_PER_INSTANCE_DATA:
			return D3D11_INPUT_PER_INSTANCE_DATA;
		default:
			break;
		}
		return D3D11_INPUT_PER_VERTEX_DATA;
	}

	inline D3D11_USAGE _ConvertUsage(USAGE usage)
	{
		switch (usage)
		{
		case Cjing3D::USAGE_DEFAULT:
			return D3D11_USAGE_DEFAULT;
		case Cjing3D::USAGE_IMMUTABLE:
			return D3D11_USAGE_IMMUTABLE;
		case Cjing3D::USAGE_DYNAMIC:
			return D3D11_USAGE_DYNAMIC;
		case Cjing3D::USAGE_STAGING:
			return D3D11_USAGE_STAGING;
		default:
			break;
		}
		return D3D11_USAGE_DEFAULT;
	}

	inline U32 _ParseBindFlags(U32 value)
	{
		U32 flags = 0;
		if (value & BIND_VERTEX_BUFFER)
			flags |= D3D11_BIND_VERTEX_BUFFER;
		if (value & BIND_INDEX_BUFFER)
			flags |= D3D11_BIND_INDEX_BUFFER;
		if (value & BIND_CONSTANT_BUFFER)
			flags |= D3D11_BIND_CONSTANT_BUFFER;
		if (value & BIND_SHADER_RESOURCE)
			flags |= D3D11_BIND_SHADER_RESOURCE;
		if (value & BIND_STREAM_OUTPUT)
			flags |= D3D11_BIND_STREAM_OUTPUT;
		if (value & BIND_RENDER_TARGET)
			flags |= D3D11_BIND_RENDER_TARGET;
		if (value & BIND_DEPTH_STENCIL)
			flags |= D3D11_BIND_DEPTH_STENCIL;
		if (value & BIND_UNORDERED_ACCESS)
			flags |= D3D11_BIND_UNORDERED_ACCESS;

		return flags;
	}

	inline U32 _ParseCPUAccessFlags(U32 value)
	{
		U32 flags = 0;
		if (value & CPU_ACCESS_WRITE)
			flags |= D3D11_CPU_ACCESS_WRITE;
		if (value & CPU_ACCESS_READ)
			flags |= D3D11_CPU_ACCESS_READ;

		return flags;
	}

	inline UINT _ParseResourceMiscFlags(UINT value)
	{
		UINT flags = 0;
		if (value & RESOURCE_MISC_SHARED)
			flags |= D3D11_RESOURCE_MISC_SHARED;
		if (value & RESOURCE_MISC_TEXTURECUBE)
			flags |= D3D11_RESOURCE_MISC_TEXTURECUBE;
		if (value & RESOURCE_MISC_DRAWINDIRECT_ARGS)
			flags |= D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS;
		if (value & RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS)
			flags |= D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
		if (value & RESOURCE_MISC_BUFFER_STRUCTURED)
			flags |= D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		if (value & RESOURCE_MISC_TILED)
			flags |= D3D11_RESOURCE_MISC_TILED;

		return flags;
	}
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
	mViewport.mWidth = static_cast<F32>(mScreenSize[0]);
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
	ID3D11RenderTargetView* renderTargetView = &mSwapChain->GetRenderTargetView();

	float clearColor[4] = {0.0f, 0.0f, 0.0f, 1.0f};
	auto& deviceContext = GetDeviceContext(GraphicsThread_IMMEDIATE);
	deviceContext.OMSetRenderTargets(1, &renderTargetView, 0);
	deviceContext.ClearRenderTargetView(renderTargetView, clearColor);
}

void GraphicsDeviceD3D11::PresentEnd()
{
	mSwapChain->Present(mIsVsync);

	auto& deviceContext = GetDeviceContext(GraphicsThread_IMMEDIATE);
	deviceContext.OMSetRenderTargets(0, nullptr, nullptr);
	deviceContext.ClearState();
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

HRESULT GraphicsDeviceD3D11::CreateDepthStencilState(const DepthStencilStateDesc & desc, DepthStencilState & state)
{
	D3D11_DEPTH_STENCIL_DESC depthDesc = {};
	depthDesc.DepthEnable		= desc.mDepthEnable;
	depthDesc.DepthWriteMask	= _ConvertDepthWriteMask(desc.mDepthWriteMask);
	depthDesc.DepthFunc			= _ConvertComparisonFunc(desc.mDepthFunc);
	depthDesc.StencilEnable		= desc.mStencilEnable;
	depthDesc.StencilReadMask	= desc.mStencilReadMask;
	depthDesc.StencilWriteMask	= desc.mStencilWriteMask;

	depthDesc.FrontFace.StencilFunc			 = _ConvertComparisonFunc(desc.mFrontFace.mDepthFunc);
	depthDesc.FrontFace.StencilPassOp		 = _ConvertStencilOp(desc.mFrontFace.mStencilPassOp);
	depthDesc.FrontFace.StencilFailOp		 = _ConvertStencilOp(desc.mFrontFace.mStencilFailOp);
	depthDesc.FrontFace.StencilDepthFailOp   = _ConvertStencilOp(desc.mFrontFace.mStencilDepthFailOp);

	depthDesc.BackFace.StencilFunc			 = _ConvertComparisonFunc(desc.mBackFace.mDepthFunc);
	depthDesc.BackFace.StencilPassOp		 = _ConvertStencilOp(desc.mBackFace.mStencilPassOp);
	depthDesc.BackFace.StencilFailOp		 = _ConvertStencilOp(desc.mBackFace.mStencilFailOp);
	depthDesc.BackFace.StencilDepthFailOp    = _ConvertStencilOp(desc.mBackFace.mStencilDepthFailOp);

	auto& depthStencilState = state.GetStatePtr();
	return mDevice->CreateDepthStencilState(&depthDesc, depthStencilState.ReleaseAndGetAddressOf());
}

HRESULT GraphicsDeviceD3D11::CreateBlendState(const BlendStateDesc & desc, BlendState & state)
{
	D3D11_BLEND_DESC blendDesc = {};
	blendDesc.AlphaToCoverageEnable = desc.mAlphaToCoverageEnable;
	blendDesc.IndependentBlendEnable = desc.mIndependentBlendEnable;

	for (int i = 0; i < 8; i++)
	{
		blendDesc.RenderTarget[i].BlendEnable = desc.mRenderTarget[i].mBlendEnable;
		blendDesc.RenderTarget[i].SrcBlend = _ConvertBlend(desc.mRenderTarget[i].mSrcBlend);
		blendDesc.RenderTarget[i].DestBlend = _ConvertBlend(desc.mRenderTarget[i].mDstBlend);
		blendDesc.RenderTarget[i].BlendOp = _ConvertBlendOp(desc.mRenderTarget[i].mBlendOp);
		blendDesc.RenderTarget[i].SrcBlendAlpha = _ConvertBlend(desc.mRenderTarget[i].mSrcBlendAlpha);
		blendDesc.RenderTarget[i].DestBlendAlpha = _ConvertBlend(desc.mRenderTarget[i].mDstBlendAlpha);
		blendDesc.RenderTarget[i].BlendOpAlpha = _ConvertBlendOp(desc.mRenderTarget[i].mBlendOpAlpha);

		blendDesc.RenderTarget[i].RenderTargetWriteMask = _ParseColorWriteMask(desc.mRenderTarget[i].mRenderTargetWriteMask);
	}

	auto& blendState = state.GetStatePtr();
	return mDevice->CreateBlendState(&blendDesc, blendState.ReleaseAndGetAddressOf());
}

HRESULT GraphicsDeviceD3D11::CreateRasterizerState(const RasterizerStateDesc & desc, RasterizerState & state)
{
	D3D11_RASTERIZER_DESC rasterizerDesc = {};

	rasterizerDesc.FillMode = _ConvertFillMode(desc.mFillMode);
	rasterizerDesc.CullMode = _ConvertCullMode(desc.mCullMode);
	rasterizerDesc.FrontCounterClockwise = desc.mFrontCounterClockwise;

	rasterizerDesc.DepthBias = desc.mDepthBias;
	rasterizerDesc.DepthBiasClamp = desc.mDepthBiasClamp;
	rasterizerDesc.SlopeScaledDepthBias = desc.mSlopeScaleDepthBias;
	rasterizerDesc.DepthClipEnable = desc.mDepthClipEnable;

	rasterizerDesc.ScissorEnable = true;
	rasterizerDesc.MultisampleEnable = desc.mMultisampleEnable;
	rasterizerDesc.AntialiasedLineEnable = desc.mAntialiaseLineEnable;

	auto& rasterizerState = state.GetStatePtr();
	return mDevice->CreateRasterizerState(&rasterizerDesc, rasterizerState.ReleaseAndGetAddressOf());
}

// 创建顶点着色器
HRESULT GraphicsDeviceD3D11::CreateVertexShader(const void * bytecode, size_t length, VertexShader & vertexShader)
{
	vertexShader.mByteCode.mByteLength = length;
	vertexShader.mByteCode.mByteData = new BYTE[length];
	memcpy(vertexShader.mByteCode.mByteData, bytecode, length);

	return mDevice->CreateVertexShader(bytecode, length, nullptr, vertexShader.mResourceD3D11.ReleaseAndGetAddressOf());
}

// 创建输入布局
HRESULT GraphicsDeviceD3D11::CreateInputLayout(VertexLayoutDesc* desc, U32 numElements, const void * shaderBytecode, size_t shaderLength, InputLayout & inputLayout)
{
	D3D11_INPUT_ELEMENT_DESC* inputLayoutdescs = new D3D11_INPUT_ELEMENT_DESC[numElements];
	for (int i = 0; i < numElements; i++)
	{
		inputLayoutdescs[i].SemanticName	= desc[i].mSemanticName;
		inputLayoutdescs[i].SemanticIndex = desc[i].mSemanticIndex;
		inputLayoutdescs[i].Format = _ConvertFormat(desc[i].mFormat);
		inputLayoutdescs[i].InputSlot  = desc[i].mInputSlot;
		inputLayoutdescs[i].AlignedByteOffset = desc[i].mAlignedByteOffset;
		inputLayoutdescs[i].InputSlotClass = _ConvertInputClassification(desc[i].mInputSlotClass);
		inputLayoutdescs[i].InstanceDataStepRate  = desc[i].mInstanceDataStepRate;
	}

	auto& inputLayputResource = inputLayout.GetStatePtr();
	return mDevice->CreateInputLayout(inputLayoutdescs, numElements, shaderBytecode, shaderLength, inputLayputResource.ReleaseAndGetAddressOf());
}

HRESULT GraphicsDeviceD3D11::CreateBuffer(const GPUBufferDesc * desc, ConstantBuffer & buffer)
{
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.ByteWidth = desc->mByteWidth;
	bufferDesc.Usage = _ConvertUsage(desc->mUsage);
	bufferDesc.BindFlags = _ParseBindFlags(desc->mBindFlags);
	bufferDesc.CPUAccessFlags = _ParseCPUAccessFlags(desc->mCPUAccessFlags);
	bufferDesc.MiscFlags = _ParseResourceMiscFlags(desc->mMiscFlags);
	bufferDesc.StructureByteStride = desc->mStructureByteStride;

	auto& bufferPtr = buffer.GetBufferPtr();
	return mDevice->CreateBuffer(&bufferDesc, nullptr, bufferPtr.ReleaseAndGetAddressOf());
}

void GraphicsDeviceD3D11::UpdateBuffer(ConstantBuffer & buffer, const void * data, U32 dataSize)
{
	const auto& desc = buffer.GetDesc();
	Debug::CheckAssertion(desc.mUsage != USAGE_IMMUTABLE, "Cannot update IMMUTABLE Buffer");
	Debug::CheckAssertion(desc.mByteWidth >= dataSize, "Data size is unable");

	dataSize = std::min(dataSize, desc.mByteWidth);
	if (dataSize <= 0) {
		return;
	}

	// 仅当usage为USAGE_DYNAMIC时，CPU使用map/unmap写数据
	if (desc.mUsage == USAGE_DYNAMIC)
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		HRESULT result = GetDeviceContext(GraphicsThread_IMMEDIATE).Map(&buffer.GetBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		Debug::ThrowIfFailed(result, "Failed to map buffer:%08x", result);

		memcpy(mappedResource.pData, data, dataSize);

		GetDeviceContext(GraphicsThread_IMMEDIATE).Unmap(&buffer.GetBuffer(), 0);
	}
	else
	{
		D3D11_BOX box = {};
		box.left = 0;
		box.right = dataSize;
		box.top = 0;
		box.bottom = 1;
		box.front = 0;
		box.back = 1;

		GetDeviceContext(GraphicsThread_IMMEDIATE).UpdateSubresource(&buffer.GetBuffer(), 0, &box, data, 0, 0);
	}
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
