#pragma once

#include "renderer\RHI\device.h"
#include "renderer\RHI\d3d11\includeD3D11.h"
#include "renderer\RHI\d3d11\swapChainD3D11.h"
#include "renderer\RHI\rhiShader.h"
#include "renderer\RHI\rhiResource.h"

namespace Cjing3D
{
class GameWindow;

class GraphicsDeviceD3D11 : public GraphicsDevice
{
public:
	GraphicsDeviceD3D11(const GameWindow& gameWindow, FORMAT backbufferFormat, bool debugLayer);
	~GraphicsDeviceD3D11();

	virtual void PresentBegin(CommandList cmd);
	virtual void PresentEnd(CommandList cmd);
	virtual void BeginEvent(CommandList cmd, const std::string& name);
	virtual void EndEvent(CommandList cmd);
	virtual void SetResourceName(GPUResource& resource, const std::string& name);

	virtual CommandList GetCommandList();
	virtual void SubmitCommandList();
	virtual void ResetCommandList(CommandList cmd);

	virtual HRESULT CreateDepthStencilState(const DepthStencilStateDesc& desc, DepthStencilState& state);
	virtual HRESULT CreateBlendState(const BlendStateDesc& desc, BlendState& state);
	virtual HRESULT CreateRasterizerState(const RasterizerStateDesc& desc, RasterizerState& state);
	virtual HRESULT CreateInputLayout(VertexLayoutDesc* desc, U32 numElements, Shader& shader, InputLayout& inputLayout);
	virtual HRESULT CreateShader(SHADERSTAGES stage, const void* bytecode, size_t length, Shader& shader);
	virtual HRESULT CreateBuffer(const GPUBufferDesc* desc, GPUBuffer& buffer, const SubresourceData* initialData);
	virtual HRESULT CreateSamplerState(const SamplerDesc* desc, SamplerState& state);
	virtual HRESULT CreateTexture2D(const TextureDesc* desc, const SubresourceData* data, Texture2D& texture2D);
	virtual I32 CreateRenderTargetView(Texture2D& texture);
	virtual I32 CreateShaderResourceView(Texture2D& texture, U32 arraySlice = 0, U32 arrayCount = -1, U32 firstMip = 0, U32 mipLevel = -1);
	virtual I32 CreateDepthStencilView(Texture2D& texture, U32 arraySlice = 0, U32 arrayCount = -1);
	virtual I32 CreateUnordereddAccessView(Texture2D& texture, U32 arraySlice = 0, U32 arrayCount = -1, U32 firstMip = 0);
	virtual void CreateRenderBehavior(RenderBehaviorDesc& desc, RenderBehavior& behavior);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////

	virtual void BindViewports(CommandList cmd, const ViewPort* viewports, U32 numViewports);
	virtual void BindConstantBuffer(CommandList cmd, SHADERSTAGES stage, GPUBuffer& buffer, U32 slot);
	virtual void BindIndexBuffer(CommandList cmd, GPUBuffer& buffer, IndexFormat format, U32 offset);
	virtual void BindVertexBuffer(CommandList cmd, GPUBuffer* const* buffer, U32 slot, U32 num, const U32* strides, const U32* offsets);
	virtual void BindSamplerState(CommandList cmd, SHADERSTAGES stage, SamplerState& state, U32 slot);
	virtual void BindRenderTarget(CommandList cmd, UINT numView, Texture2D* const* texture2D, Texture2D* depthStencilTexture, I32 subresourceIndex = -1);
	virtual void BindGPUResource(CommandList cmd, SHADERSTAGES stage, const GPUResource* resource, U32 slot, I32 subresourceIndex = -1);
	virtual void BindGPUResources(CommandList cmd, SHADERSTAGES stage, const GPUResource* const* resource, U32 slot, U32 count);
	virtual void UnbindGPUResources(CommandList cmd, U32 slot, U32 count);
	virtual void CopyGPUResource(CommandList cmd, GPUResource& texDst, GPUResource& texSrc);
	virtual void UpdateBuffer(CommandList cmd, GPUBuffer& buffer, const void* data, I32 dataSize = -1);
	virtual void ClearVertexBuffer(CommandList cmd);
	virtual void ClearRenderTarget(CommandList cmd, Texture2D& texture, F32x4 color);
	virtual void ClearDepthStencil(CommandList cmd, Texture2D& texture, UINT clearFlag, F32 depth, U8 stencil, I32 subresourceIndex = -1);
	virtual void BeginRenderBehavior(CommandList cmd, RenderBehavior& behavior);
	virtual void EndRenderBehavior(CommandList cmd);
	virtual void BindScissorRects(CommandList cmd, U32 num, const RectInt* rects);

	virtual void BindComputeShader(CommandList cmd, ShaderPtr computeShader);
	virtual void Dispatch(CommandList cmd, U32 threadGroupCountX, U32 threadGroupCountY, U32 threadGroupCountZ);
	virtual void DispatchIndirect(CommandList cmd, const GPUBuffer* buffer, U32 offset);
	virtual void BindUAV(CommandList cmd, GPUResource* const resource, U32 slot, I32 subresourceIndex = -1);
	virtual void BindUAVs(CommandList cmd, GPUResource* const* resource, U32 slot, U32 count);
	virtual void UnBindUAVs(CommandList cmd, U32 slot, U32 count);
	virtual void UnBindAllUAVs(CommandList cmd);

	virtual void BindPipelineState(CommandList cmd, const PipelineState* state);
	virtual void RefreshPipelineState(CommandList cmd);
	virtual void Draw(CommandList cmd, UINT vertexCount, UINT startVertexLocation);
	virtual void DrawIndexed(CommandList cmd, UINT indexCount, UINT startIndexLocation) ;
	virtual void DrawInstanced(CommandList cmd, U32 vertexCountPerInstance, U32 instanceCount, U32 startVertexLocation, U32 startInstanceLocation);
	virtual void DrawIndexedInstanced(CommandList cmd, U32 indexCount, U32 instanceCount, U32 startIndexLocation, U32 baseVertexLocation, U32 startInstanceLocation);
	virtual void DrawInstancedIndirect(CommandList cmd, const GPUBuffer* buffer, U32 offset);

	// allocate
	virtual GPUAllocation AllocateGPU(CommandList cmd, size_t dataSize);
	virtual void UnAllocateGPU(CommandList cmd);
	void CommitAllocations(CommandList cmd);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////

	// query
	virtual HRESULT CreateQuery(const GPUQueryDesc& desc, GPUQuery& query);
	virtual void BeginQuery(CommandList cmd, GPUQuery& query);
	virtual void EndQuery(CommandList cmd, GPUQuery& query);
	virtual HRESULT ReadQuery(GPUQuery& query, GPUQueryResult& result);
	virtual void Map(const GPUResource* resource, GPUResourceMapping& mapping);
	virtual void Unmap(const GPUResource* resource);

	// status
	inline ID3D11Device& GetDevice() { return *mDevice.Get(); }
	ID3D11DeviceContext& GetImmediateDeviceContext() { return *mImmediateContext.Get(); }
	ID3D11DeviceContext& GetDeviceContext(CommandList cmd) { return *mDeviceContexts[cmd].Get();}

	virtual void SetResolution(const U32x2 size);

private:
	ComPtr<ID3D11Device> mDevice;
	ComPtr<ID3D11DeviceContext> mImmediateContext;
	ComPtr<ID3D11DeviceContext> mDeviceContexts[MAX_COMMANDLIST_COUNT];
	ComPtr<ID3DUserDefinedAnnotation> mUserDefinedAnnotations[MAX_COMMANDLIST_COUNT];
	D3D_FEATURE_LEVEL mFeatureLevel;
	std::unique_ptr< SwapChainD3D11> mSwapChain;
	std::atomic<CommandList> mCommandListCount{ 0 };
	ComPtr<ID3D11CommandList> mCommandLists[MAX_COMMANDLIST_COUNT];
	GPUAllocator mGPUAllocator[MAX_COMMANDLIST_COUNT];

	// prev pipeline state
	struct PrevPipelineStateInfo
	{
		ID3D11VertexShader* mPrevVertexShader           = nullptr;
		ID3D11PixelShader* mPrevPixelShader				= nullptr;
		ID3D11ComputeShader* mPrevComputeShader			= nullptr;
		ID3D11HullShader* mPrevHullShader				= nullptr;
		ID3D11DomainShader* mPrevDomainShader			= nullptr;
		ID3D11DepthStencilState* mPrevDepthStencilState = nullptr;
		ID3D11BlendState* mPrevBlendState				= nullptr;
		ID3D11RasterizerState* mPrevRasterizerState		= nullptr;
		ID3D11InputLayout* mPrevInputLayout				= nullptr;
		PRIMITIVE_TOPOLOGY mPrevPrimitiveTopology		= UNDEFINED_TOPOLOGY;
	};
	PrevPipelineStateInfo mPrevPipelineStates[MAX_COMMANDLIST_COUNT];
	void ClearPrevStates(CommandList cmd);

	const PipelineState* mBindedPSO[MAX_COMMANDLIST_COUNT] = {nullptr};
	bool mDirtyPSO[MAX_COMMANDLIST_COUNT] = {false};
	
};

}