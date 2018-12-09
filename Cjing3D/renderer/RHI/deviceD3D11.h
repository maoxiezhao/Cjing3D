#pragma once

#include "renderer\includeD3D11.h"
#include "renderer\RHI\device.h"
#include "renderer\RHI\swapChainD3D11.h"

namespace Cjing3D
{

class GraphicsDeviceD3D11 : public GraphicsDevice
{
public:
	GraphicsDeviceD3D11(HWND window, bool fullScreen, bool debugLayer);

	virtual void Initialize();
	virtual void Uninitialize();

	virtual void PresentBegin();
	virtual void PresentEnd();

	virtual void BindViewports(const ViewPort* viewports, U32 numViewports, GraphicsThread threadID);

	/** Creating function */
	virtual HRESULT CreateDepthStencilState(const DepthStencilStateDesc& desc, DepthStencilState& state);
	virtual HRESULT CreateBlendState(const BlendStateDesc& desc, BlendState& state);
	virtual HRESULT CreateRasterizerState(const RasterizerStateDesc& desc, RasterizerState& state);

	virtual HRESULT CreateVertexShader(const void* bytecode, size_t length, VertexShader& vertexShader);
	virtual HRESULT CreateInputLayout(VertexLayoutDesc* desc, U32 numElements, const void* shaderBytecode, size_t shaderLength, InputLayout& inputLayout);

	virtual HRESULT CreateBuffer(const GPUBufferDesc* desc, ConstantBuffer& buffer);
	virtual void UpdateBuffer(ConstantBuffer& buffer, const void* data, U32 dataSize);

	virtual HRESULT CreateSamplerState(const SamplerDesc* desc, SamplerState& state);

	virtual HRESULT CreateTexture2D(const TextureDesc* desc, const SubresourceData* data, RhiTexture2D& texture2D);

	virtual HRESULT CreateRenderTargetView(RhiTexture2D& texture);
	virtual HRESULT CreateShaderResourceView(RhiTexture2D& texture);
	virtual HRESULT CreateDepthStencilView(RhiTexture2D& texture);

	virtual void DestoryGPUResource(GPUResource& resource);

	ID3D11DeviceContext& GetDeviceContext(GraphicsThread type) {
		return *mDeviceContext[static_cast<U32>(type)].Get();
	}

	void SetViewport(ViewPort viewport);

private:
	void InitializeDevice();

private:
	HWND mWindow;
	bool mDebugLayer;

	ComPtr<ID3D11Device> mDevice;
	ComPtr<ID3D11DeviceContext> mDeviceContext[GraphicsThread_COUNT];
	ID3DUserDefinedAnnotation*	mUserDefinedAnnotations[GraphicsThread_COUNT];
	D3D_FEATURE_LEVEL mFeatureLevel;
	std::unique_ptr< SwapChainD3D11> mSwapChain;
};

}