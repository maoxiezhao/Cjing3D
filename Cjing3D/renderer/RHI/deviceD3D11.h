#pragma once

#include "renderer\includeD3D11.h"
#include "renderer\RHI\device.h"
#include "renderer\RHI\swapChainD3D11.h"
#include "renderer\RHI\rhiShader.h"

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
	virtual HRESULT CreatePixelShader(const void* bytecode, size_t length, PixelShader &pixelShader);

	virtual HRESULT CreateBuffer(const GPUBufferDesc* desc, GPUBuffer& buffer, const SubresourceData* initialData);
	virtual void UpdateBuffer(GPUBuffer& buffer, const void* data, U32 dataSize);
	virtual void BindConstantBuffer(SHADERSTAGES stage, GPUBuffer& buffer, U32 slot);

	virtual void BindIndexBuffer(GPUBuffer& buffer, IndexFormat format, U32 offset);
	virtual void BindVertexBuffer(GPUBuffer* const* buffer, U32 slot, U32 num, const U32* strides, const U32* offsets);

	virtual HRESULT CreateSamplerState(const SamplerDesc* desc, SamplerState& state);
	virtual void BindSamplerState(SHADERSTAGES stage, SamplerState& state, U32 slot);

	virtual HRESULT CreateTexture2D(const TextureDesc* desc, const SubresourceData* data, RhiTexture2D& texture2D);
	virtual void DestroyTexture2D(RhiTexture2D& texture2D);

	virtual void BindRenderTarget(UINT numView, RhiTexture2D* const *texture2D, RhiTexture2D* depthStencilTexture);

	virtual HRESULT CreateRenderTargetView(RhiTexture2D& texture);
	virtual HRESULT CreateShaderResourceView(RhiTexture2D& texture);
	virtual HRESULT CreateDepthStencilView(RhiTexture2D& texture);

	virtual void ClearRenderTarget(RhiTexture2D& texture, F32x4 color);
	virtual void ClearDepthStencil(RhiTexture2D& texture, UINT clearFlag, F32 depth, U8 stencil);

	virtual void BindGPUResource(SHADERSTAGES stage, GPUResource& resource, U32 slot);
	virtual void BindGPUResources(SHADERSTAGES stage, GPUResource* const* resource, U32 slot, U32 count);
	virtual void DestroyGPUResource(GPUResource& resource);

	virtual void BindShaderInfoState(ShaderInfoState state);

	void ClearPrevStates();

	virtual void Draw(UINT vertexCount, UINT startVertexLocation);
	virtual void DrawIndexed(UINT indexCount, UINT startIndexLocation) ;
	virtual void DrawIndexedInstances(U32 indexCount, U32 instanceCount, U32 startIndexLocation, U32 baseVertexLocation, U32 startInstanceLocation);

	ID3D11DeviceContext& GetDeviceContext(GraphicsThread type) {
		return *mDeviceContext[static_cast<U32>(type)].Get();
	}

	void SetViewport(ViewPort viewport);

	virtual GPUAllocation AllocateGPU(size_t dataSize);

private:
	void InitializeDevice();

	// GPU Buffer����
	struct GPUAllocator
	{
		std::unique_ptr<GPUBuffer> buffer;
		size_t byteOffset = 0;
		uint64_t residentFrame = 0;
		bool dirty = false;

		size_t GetDataSize() { return buffer->GetDesc().mByteWidth; }
	};
	GPUAllocator mGPUAllocator;
	GPUBufferDesc mGPUAllocatorDesc;

	void CommitAllocations();

private:
	HWND mWindow;
	bool mDebugLayer;

	ComPtr<ID3D11Device> mDevice;
	ComPtr<ID3D11DeviceContext> mDeviceContext[GraphicsThread_COUNT];
	ID3DUserDefinedAnnotation*	mUserDefinedAnnotations[GraphicsThread_COUNT];
	D3D_FEATURE_LEVEL mFeatureLevel;
	std::unique_ptr< SwapChainD3D11> mSwapChain;

	// prev state
	ID3D11VertexShader* mPrevVertexShader = nullptr;
	ID3D11PixelShader* mPrevPixelShader = nullptr;
	ID3D11InputLayout* mPrevInputLayout = nullptr;
	PRIMITIVE_TOPOLOGY mPrevPrimitiveTopology = UNDEFINED_TOPOLOGY;
};

}