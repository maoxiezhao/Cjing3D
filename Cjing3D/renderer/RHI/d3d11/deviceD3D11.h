#pragma once

#include "renderer\RHI\device.h"
#include "renderer\RHI\d3d11\includeD3D11.h"
#include "renderer\RHI\d3d11\swapChainD3D11.h"
#include "renderer\RHI\rhiShader.h"
#include "renderer\RHI\rhiResource.h"

namespace Cjing3D
{

class GraphicsDeviceD3D11 : public GraphicsDevice
{
public:
	GraphicsDeviceD3D11(HWND window, bool fullScreen, FORMAT backbufferFormat, bool debugLayer);

	virtual void Initialize();
	virtual void Uninitialize();
	virtual void PresentBegin();
	virtual void PresentEnd();
	virtual void BeginEvent(const std::string& name);
	virtual void EndEvent();

	virtual void BindViewports(const ViewPort* viewports, U32 numViewports, GraphicsThread threadID);

	/** Creating function */
	virtual HRESULT CreateDepthStencilState(const DepthStencilStateDesc& desc, DepthStencilState& state);
	virtual HRESULT CreateBlendState(const BlendStateDesc& desc, BlendState& state);
	virtual HRESULT CreateRasterizerState(const RasterizerStateDesc& desc, RasterizerState& state);
	virtual HRESULT CreateInputLayout(VertexLayoutDesc* desc, U32 numElements, Shader& shader, InputLayout& inputLayout);
	virtual HRESULT CreateShader(SHADERSTAGES stage, const void* bytecode, size_t length, Shader& shader);

	virtual HRESULT CreateBuffer(const GPUBufferDesc* desc, GPUBuffer& buffer, const SubresourceData* initialData);
	virtual void UpdateBuffer(GPUBuffer& buffer, const void* data, I32 dataSize = -1);
	virtual void BindConstantBuffer(SHADERSTAGES stage, GPUBuffer& buffer, U32 slot);

	virtual void BindIndexBuffer(GPUBuffer& buffer, IndexFormat format, U32 offset);
	virtual void BindVertexBuffer(GPUBuffer* const* buffer, U32 slot, U32 num, const U32* strides, const U32* offsets);
	virtual void ClearVertexBuffer();

	virtual HRESULT CreateSamplerState(const SamplerDesc* desc, SamplerState& state);
	virtual void BindSamplerState(SHADERSTAGES stage, SamplerState& state, U32 slot);

	virtual HRESULT CreateTexture2D(const TextureDesc* desc, const SubresourceData* data, Texture2D& texture2D);
	virtual I32 CreateRenderTargetView(Texture2D& texture);
	virtual I32 CreateShaderResourceView(Texture2D& texture, U32 arraySlice = 0, U32 arrayCount = -1, U32 firstMip = 0, U32 mipLevel = -1);
	virtual I32 CreateDepthStencilView(Texture2D& texture, U32 arraySlice = 0, U32 arrayCount = -1);
	virtual I32 CreateUnordereddAccessView(Texture2D& texture, U32 arraySlice = 0, U32 arrayCount = -1, U32 firstMip = 0);

	virtual void BindRenderTarget(UINT numView, Texture2D* const* texture2D, Texture2D* depthStencilTexture, I32 subresourceIndex = -1);
	virtual void ClearRenderTarget(Texture2D& texture, F32x4 color);
	virtual void ClearDepthStencil(Texture2D& texture, UINT clearFlag, F32 depth, U8 stencil, I32 subresourceIndex = -1);

	virtual void BindGPUResource(SHADERSTAGES stage, const GPUResource* resource, U32 slot, I32 subresourceIndex = -1);
	virtual void BindGPUResources(SHADERSTAGES stage, const GPUResource* const* resource, U32 slot, U32 count);
	virtual void UnbindGPUResources(U32 slot, U32 count);
	virtual void SetResourceName(GPUResource& resource, const std::string& name);
	virtual void CopyGPUResource(GPUResource& texDst, GPUResource& texSrc);

	virtual void CreateRenderBehavior(RenderBehaviorDesc& desc, RenderBehavior& behavior);
	virtual void BeginRenderBehavior(RenderBehavior& behavior);
	virtual void EndRenderBehavior();

	virtual void BindScissorRects(U32 num, const RectInt* rects);

	// compute
	virtual void BindComputeShader(ShaderPtr computeShader);
	virtual void Dispatch(U32 threadGroupCountX, U32 threadGroupCountY, U32 threadGroupCountZ);
	virtual void DispatchIndirect(const GPUBuffer* buffer, U32 offset);
	virtual void BindUAV(GPUResource* const resource, U32 slot, I32 subresourceIndex = -1);
	virtual void BindUAVs(GPUResource* const* resource, U32 slot, U32 count);
	virtual void UnBindUAVs(U32 slot, U32 count);
	virtual void UnBindAllUAVs();

	virtual void BindPipelineState(PipelineState state);
	void ClearPrevStates();
	virtual void Draw(UINT vertexCount, UINT startVertexLocation);
	virtual void DrawIndexed(UINT indexCount, UINT startIndexLocation) ;
	virtual void DrawInstanced(U32 vertexCountPerInstance, U32 instanceCount, U32 startVertexLocation, U32 startInstanceLocation);
	virtual void DrawIndexedInstanced(U32 indexCount, U32 instanceCount, U32 startIndexLocation, U32 baseVertexLocation, U32 startInstanceLocation);
	virtual void DrawInstancedIndirect(const GPUBuffer* buffer, U32 offset);

	// query
	virtual HRESULT CreateQuery(const GPUQueryDesc& desc, GPUQuery& query) ;
	virtual void BeginQuery(GPUQuery& query);
	virtual void EndQuery(GPUQuery& query);
	virtual HRESULT ReadQuery(GPUQuery& query, GPUQueryResult& result);
	virtual void Map(const GPUResource* resource, GPUResourceMapping& mapping);
	virtual void Unmap(const GPUResource* resource);

	inline HWND GetHwnd() { return mWindow; }
	inline ID3D11Device& GetDevice() { return *mDevice.Get(); }

	ID3D11DeviceContext& GetDeviceContext(GraphicsThread type) {
		return *mDeviceContext[static_cast<U32>(type)].Get();
	}

	ID3DUserDefinedAnnotation& GetUserDefineAnnotation(GraphicsThread type) {
		return *mUserDefinedAnnotations[static_cast<U32>(type)];
	}

	void SetViewport(ViewPort viewport);

	virtual GPUAllocation AllocateGPU(size_t dataSize);
	virtual void UnAllocateGPU();


private:
	void InitializeDevice();

	// GPU Buffer∑÷≈‰
	struct GPUAllocator
	{
		GPUBuffer buffer;
		size_t byteOffset = 0;
		uint64_t residentFrame = 0;
		bool dirty = false;

		size_t GetDataSize() { return buffer.GetDesc().mByteWidth; }
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

	// prev pipeline state
	ID3D11VertexShader* mPrevVertexShader = nullptr;
	ID3D11PixelShader* mPrevPixelShader = nullptr;
	ID3D11ComputeShader* mPrevComputeShader = nullptr;
	ID3D11HullShader* mPrevHullShader = nullptr;
	ID3D11DomainShader* mPrevDomainShader = nullptr;

	ID3D11DepthStencilState* mPrevDepthStencilState = nullptr;
	ID3D11BlendState* mPrevBlendState = nullptr;
	ID3D11RasterizerState* mPrevRasterizerState = nullptr;

	ID3D11InputLayout* mPrevInputLayout = nullptr;
	PRIMITIVE_TOPOLOGY mPrevPrimitiveTopology = UNDEFINED_TOPOLOGY;
};

}