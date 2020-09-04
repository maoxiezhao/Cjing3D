#pragma once

#include "renderer\renderableCommon.h"
#include "renderer\RHI\rhiResource.h"
#include "renderer\RHI\rhiDefinition.h"
#include "renderer\RHI\rhiShader.h"

namespace Cjing3D
{
	enum GraphicsDeviceType
	{
		GraphicsDeviceType_unknown,
		GraphicsDeviceType_directx11
	};

	struct GraphicsFeatureSupport
	{
		enum GraphicsFeature
		{
			VIEWPORT_AND_RENDERTARGET_ARRAYINDEX_WITHOUT_GS,
			SUPPORT_TESSELLATION,
		};
		bool viewportAndRenderTargetArrayIndexWithoutGS_ = true;
		bool supportTessellation_ = false;
	};

	class GraphicsDevice
	{
	public:
		static const U32 MAX_COMMANDLIST_COUNT = 16;

	public:
		GraphicsDevice(GraphicsDeviceType type);
		virtual ~GraphicsDevice();

		virtual void PresentBegin(CommandList cmd) = 0;
		virtual void PresentEnd(CommandList cmd) = 0;
		virtual void BeginEvent(CommandList cmd, const std::string& name) = 0;
		virtual void EndEvent(CommandList cmd) = 0;
		virtual void SetResourceName(GPUResource& resource, const std::string& name) = 0;

		virtual CommandList GetCommandList() = 0;
		virtual void SubmitCommandList() = 0;
		virtual void ResetCommandList(CommandList cmd) = 0;

		virtual HRESULT CreateDepthStencilState(const DepthStencilStateDesc& desc, DepthStencilState& state) = 0;
		virtual HRESULT CreateBlendState(const BlendStateDesc& desc, BlendState& state) = 0;
		virtual HRESULT CreateRasterizerState(const RasterizerStateDesc& desc, RasterizerState& state) = 0;
		virtual HRESULT CreateInputLayout(VertexLayoutDesc* desc, U32 numElements, Shader& shader, InputLayout& inputLayout) = 0;
		virtual HRESULT CreateShader(SHADERSTAGES stage, const void* bytecode, size_t length, Shader& shader) = 0;
		virtual HRESULT CreateBuffer(const GPUBufferDesc* desc, GPUBuffer& buffer, const SubresourceData* initialData) = 0;
		virtual HRESULT CreateSamplerState(const SamplerDesc* desc, SamplerState& state) = 0;
		virtual HRESULT CreateTexture2D(const TextureDesc* desc, const SubresourceData* data, Texture2D& texture2D) = 0;
		virtual I32 CreateRenderTargetView(Texture2D& texture) = 0;
		virtual I32 CreateShaderResourceView(Texture2D& texture, U32 arraySlice = 0, U32 arrayCount = -1, U32 firstMip = 0, U32 mipLevel = -1) = 0;
		virtual I32 CreateDepthStencilView(Texture2D& texture, U32 arraySlice = 0, U32 arrayCount = -1) = 0;
		virtual I32 CreateUnordereddAccessView(Texture2D& texture, U32 arraySlice = 0, U32 arrayCount = -1, U32 firstMip = 0) = 0;
		virtual void CreateRenderBehavior(RenderBehaviorDesc& desc, RenderBehavior& behavior) = 0;
		void CreatePipelineState(PipelineStateDesc& desc, PipelineState& state);

		//////////////////////////////////////////////////////////////////////////////////////////////////////////

		virtual void BindViewports(CommandList cmd, const ViewPort* viewports, U32 numViewports) = 0;
		virtual void BindConstantBuffer(CommandList cmd, SHADERSTAGES stage, GPUBuffer& buffer, U32 slot) = 0;
		virtual void BindIndexBuffer(CommandList cmd, GPUBuffer& buffer, IndexFormat format, U32 offset) = 0;
		virtual void BindVertexBuffer(CommandList cmd, GPUBuffer* const* buffer, U32 slot, U32 num, const U32* strides, const U32* offsets) = 0;
		virtual void BindSamplerState(CommandList cmd, SHADERSTAGES stage, SamplerState& state, U32 slot) = 0;
		virtual void BindRenderTarget(CommandList cmd, UINT numView, Texture2D* const* texture2D, Texture2D* depthStencilTexture, I32 subresourceIndex = -1) = 0;
		virtual void BindGPUResource(CommandList cmd, SHADERSTAGES stage, const GPUResource* resource, U32 slot, I32 subresourceIndex = -1) = 0;
		virtual void BindGPUResources(CommandList cmd, SHADERSTAGES stage, const GPUResource* const* resource, U32 slot, U32 count) = 0;
		virtual void UnbindGPUResources(CommandList cmd, U32 slot, U32 count) = 0;
		virtual void CopyGPUResource(CommandList cmd, GPUResource& texDst, GPUResource& texSrc) = 0;
		virtual void UpdateBuffer(CommandList cmd, GPUBuffer& buffer, const void* data, I32 dataSize = -1) = 0;
		virtual void ClearVertexBuffer(CommandList cmd) = 0;
		virtual void ClearRenderTarget(CommandList cmd, Texture2D& texture, F32x4 color) = 0;
		virtual void ClearDepthStencil(CommandList cmd, Texture2D& texture, UINT clearFlag, F32 depth, U8 stencil, I32 subresourceIndex = -1) = 0;
		virtual void BeginRenderBehavior(CommandList cmd, RenderBehavior& behavior) = 0;
		virtual void EndRenderBehavior(CommandList cmd) = 0;
		virtual void BindScissorRects(CommandList cmd, U32 num, const RectInt* rects) = 0;

		virtual void BindComputeShader(CommandList cmd, ShaderPtr computeShader) = 0;
		virtual void Dispatch(CommandList cmd, U32 threadGroupCountX, U32 threadGroupCountY, U32 threadGroupCountZ) = 0;
		virtual void DispatchIndirect(CommandList cmd, const GPUBuffer* buffer, U32 offset) = 0;
		virtual void BindUAV(CommandList cmd, GPUResource* const resource, U32 slot, I32 subresourceIndex = -1) = 0;
		virtual void BindUAVs(CommandList cmd, GPUResource* const* resource, U32 slot, U32 count) = 0;
		virtual void UnBindUAVs(CommandList cmd, U32 slot, U32 count) = 0;
		virtual void UnBindAllUAVs(CommandList cmd) = 0;

		virtual void BindPipelineState(CommandList cmd, const PipelineState* state) = 0;
		virtual void RefreshPipelineState(CommandList cmd) = 0;
		virtual void Draw(CommandList cmd, UINT vertexCount, UINT startVertexLocation) = 0;
		virtual void DrawIndexed(CommandList cmd, UINT indexCount, UINT startIndexLocation) = 0;
		virtual void DrawInstanced(CommandList cmd, U32 vertexCountPerInstance, U32 instanceCount, U32 startVertexLocation, U32 startInstanceLocation) = 0;
		virtual void DrawIndexedInstanced(CommandList cmd, U32 indexCount, U32 instanceCount, U32 startIndexLocation, U32 baseVertexLocation, U32 startInstanceLocation) = 0;
		virtual void DrawInstancedIndirect(CommandList cmd, const GPUBuffer* buffer, U32 offset) = 0;

		//////////////////////////////////////////////////////////////////////////////////////////////////////////

		// query
		virtual HRESULT CreateQuery(const GPUQueryDesc& desc, GPUQuery& query) = 0;
		virtual void BeginQuery(CommandList cmd, GPUQuery& query) = 0;
		virtual void EndQuery(CommandList cmd, GPUQuery& query) = 0;
		virtual HRESULT ReadQuery(GPUQuery& query, GPUQueryResult& result) = 0;
		virtual void Map(const GPUResource* resource, GPUResourceMapping& mapping) = 0;
		virtual void Unmap(const GPUResource* resource) = 0;

		// status
		void SetViewport(ViewPort viewport) { mViewport = viewport; }
		ViewPort GetViewport()const { return mViewport; }
		U32 GetFormatStride(FORMAT value) const;
		bool IsFormatUnorm(FORMAT value)const;
		FORMAT GetBackBufferFormat()const { return mBackBufferFormat; }
		U32x2 GetScreenSize()const { return mScreenSize; }
		XMMATRIX GetScreenProjection()const { return XMMatrixOrthographicOffCenterLH(0, (F32)mScreenSize[0], (F32)mScreenSize[1], 0, -1, 1); }
		GraphicsDeviceType GetGraphicsDeviceType()const { return mGraphicsDeviceType; }
		bool GetIsVsync()const { return mIsVsync; }
		void SetIsVsync(bool isVsync) { mIsVsync = isVsync; }
		bool CheckGraphicsFeatureSupport(GraphicsFeatureSupport::GraphicsFeature feature)const;
		virtual void SetResolution(const U32x2 size) = 0;

	public:
		// gpu allocation
		struct GPUAllocation
		{
			void* data = nullptr;
			GPUBuffer* buffer = nullptr;
			U32 offset = 0;

			bool IsValid()const { return data != nullptr && buffer != nullptr; }
		};
		virtual GPUAllocation AllocateGPU(CommandList cmd, size_t dataSize) = 0;
		virtual void UnAllocateGPU(CommandList cmd) = 0;

		// GPU Buffer分配
		struct GPUAllocator
		{
			GPUBuffer buffer;
			size_t byteOffset = 0;
			uint64_t residentFrame = 0;
			bool dirty = false;

			size_t GetDataSize() { return buffer.GetDesc().mByteWidth; }
		};

		template<typename T>
		std::shared_ptr<T> RegisterGraphicsDeviceChild(GraphicsDeviceChild& deviceChild)
		{
			// 重新创建了一个rhiState，原来的rhiState则会因为引用清零自动释放
			auto rhiState = std::make_shared<T>();
			deviceChild.mRhiState = rhiState;

			return rhiState;
		}
		template<typename T>
		T* GetGraphicsDeviceChildState(const GraphicsDeviceChild& deviceChild)
		{
			return static_cast<T*>(deviceChild.mRhiState.get());
		}

	protected:
		GraphicsDeviceType mGraphicsDeviceType = GraphicsDeviceType_unknown;
		bool mIsFullScreen;
		FORMAT mBackBufferFormat = FORMAT_R8G8B8A8_UNORM;
		ViewPort mViewport;
		U32x2 mScreenSize;
		bool mIsVsync = true;
		bool mDebugLayer = false;
		uint64_t mCurrentFrameCount = 0;
		GraphicsFeatureSupport mGraphicsFeatureSupport;

		struct EmptyRhiState {};
		std::shared_ptr<EmptyRhiState> mEmptyRhiState = nullptr;
	};
}