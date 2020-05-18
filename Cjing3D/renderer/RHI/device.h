#pragma once

#include "renderer\renderableCommon.h"
#include "renderer\RHI\rhiResource.h"
#include "renderer\RHI\rhiDefinition.h"
#include "renderer\RHI\rhiStructure.h"
#include "renderer\RHI\rhiShader.h"

namespace Cjing3D
{
	enum GraphicsDeviceType
	{
		GraphicsDeviceType_unknown,
		GraphicsDeviceType_directx11
	};

	class GraphicsDevice
	{
	public:
		GraphicsDevice(GraphicsDeviceType type);

		virtual void Initialize();
		virtual void Uninitialize();
		virtual void PresentBegin();
		virtual void PresentEnd();

		virtual void BeginEvent(const std::string& name) = 0;
		virtual void EndEvent() = 0;

		virtual void BindViewports(const ViewPort* viewports, U32 numViewports, GraphicsThread threadID) = 0;

		virtual HRESULT CreateDepthStencilState(const DepthStencilStateDesc& desc, DepthStencilState& state) = 0;
		virtual HRESULT CreateBlendState(const BlendStateDesc& desc, BlendState& state) = 0;
		virtual HRESULT CreateRasterizerState(const RasterizerStateDesc& desc, RasterizerState& state) = 0;
		virtual HRESULT CreateVertexShader(const void* bytecode, size_t length, VertexShader& vertexShader) = 0;
		virtual HRESULT CreateInputLayout(VertexLayoutDesc* desc, U32 numElements, const void* shaderBytecode, size_t shaderLength, InputLayout& inputLayout) = 0;
		virtual HRESULT CreatePixelShader(const void* bytecode, size_t length, PixelShader &pixelShader) = 0;
		virtual HRESULT CreateComputeShader(const void* bytecode, size_t length, ComputeShader& computeShader) = 0;
		virtual HRESULT CreateHullShader(const void* bytecode, size_t length, HullShader& hullShader) = 0;
		virtual HRESULT CreateDomainShader(const void* bytecode, size_t length, DomainShader& domainShader) = 0;

		virtual HRESULT CreateBuffer(const GPUBufferDesc* desc, GPUBuffer& buffer, const SubresourceData* initialData) = 0;
		virtual void UpdateBuffer(GPUBuffer& buffer, const void* data, U32 dataSize) = 0;
		virtual void BindConstantBuffer(SHADERSTAGES stage, GPUBuffer& buffer, U32 slot) = 0;

		virtual void BindIndexBuffer(GPUBuffer& buffer, IndexFormat format, U32 offset) = 0;
		virtual void BindVertexBuffer(GPUBuffer* const* buffer, U32 slot, U32 num, const U32* strides, const U32* offsets) = 0;
		virtual void ClearVertexBuffer() = 0;

		virtual HRESULT CreateSamplerState(const SamplerDesc* desc, SamplerState& state) = 0;
		virtual void BindSamplerState(SHADERSTAGES stage, SamplerState& state, U32 slot) = 0;

		virtual HRESULT CreateTexture2D(const TextureDesc* desc, const SubresourceData* data, RhiTexture2D& texture2D) = 0;
		virtual void CopyTexture2D(RhiTexture2D* texDst, RhiTexture2D* texSrc) = 0;

		virtual void BindRenderTarget(UINT numView, RhiTexture2D* const *texture2D, RhiTexture2D* depthStencilTexture, I32 subresourceIndex = -1) = 0;

		virtual I32 CreateRenderTargetView(RhiTexture2D& texture) = 0;
		virtual I32 CreateShaderResourceView(RhiTexture2D& texture, U32 arraySlice = 0, U32 arrayCount = -1, U32 firstMip = 0, U32 mipLevel = -1) = 0;
		virtual I32 CreateDepthStencilView(RhiTexture2D& texture, U32 arraySlice = 0, U32 arrayCount = -1) = 0;
		virtual I32 CreateUnordereddAccessView(RhiTexture2D& texture, U32 firstMip = 0) = 0;

		virtual void ClearRenderTarget(RhiTexture2D& texture, F32x4 color) = 0;
		virtual void ClearDepthStencil(RhiTexture2D& texture, UINT clearFlag, F32 depth, U8 stencil, I32 subresourceIndex = -1) = 0;

		virtual void BindGPUResource(SHADERSTAGES stage, GPUResource& resource, U32 slot, I32 subresourceIndex = -1) = 0;
		virtual void BindGPUResources(SHADERSTAGES stage, GPUResource* const* resource, U32 slot, U32 count) = 0;
		virtual void UnbindGPUResources(U32 slot, U32 count) = 0;
		virtual void SetResourceName(GPUResource& resource, const std::string& name) = 0;

		virtual void CreatePipelineState(PipelineStateDesc& desc, PipelineState& state);
		virtual void CreateRenderBehavior(RenderBehaviorDesc& desc, RenderBehavior& behavior) = 0;
		virtual void BeginRenderBehavior(RenderBehavior& behavior) = 0;
		virtual void EndRenderBehavior() = 0;

		// compute
		virtual void BindComputeShader(ComputeShaderPtr computeShader) = 0;
		virtual void Dispatch(U32 threadGroupCountX, U32 threadGroupCountY, U32 threadGroupCountZ) = 0;
		virtual void BindUAV(GPUResource* const resource, U32 slot, I32 subresourceIndex = -1) = 0;
		virtual void BindUAVs(GPUResource* const* resource, U32 slot, U32 count) = 0;
		virtual void UnBindUAVs(U32 slot, U32 count) = 0;

		virtual void BindPipelineState(PipelineState state) = 0;

		virtual void Draw(UINT vertexCount, UINT startVertexLocation) = 0;
		virtual void DrawIndexed(UINT indexCount, UINT startIndexLocation) = 0;
		virtual void DrawIndexedInstances(U32 indexCount, U32 instanceCount, U32 startIndexLocation, U32 baseVertexLocation, U32 startInstanceLocation) = 0;

		U32 GetFormatStride(FORMAT value) const;

		FORMAT GetBackBufferFormat()const {
			return mBackBufferFormat;
		}

		U32x2 GetScreenSize()const {
			return mScreenSize;
		}

		inline XMMATRIX GetScreenProjection()const {
			return XMMatrixOrthographicOffCenterLH(0, (F32)mScreenSize[0], (F32)mScreenSize[1], 0, -1, 1);
		}

		bool IsMultithreadedRendering()const {
			return mIsMultithreadedRendering;
		}

		ViewPort GetViewport()const {
			return mViewport;
		}

		struct GPUAllocation
		{
			void* data = nullptr;
			GPUBuffer* buffer = nullptr;
			U32 offset = 0;
		};
		virtual GPUAllocation AllocateGPU(size_t dataSize) = 0;
		virtual void UnAllocateGPU() = 0;

		void AddGPUResource(GPUResource* resource);
		void RemoveGPUResource(GPUResource* resource);
		void ProcessRemovedGPUResouces();

		GraphicsDeviceType GetGraphicsDeviceType()const { return mGraphicsDeviceType; }

		bool GetIsVsync()const { return mIsVsync; }
		void SetIsVsync(bool isVsync) { mIsVsync = isVsync; }

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
		FORMAT mBackBufferFormat;
		U32x2 mScreenSize;
		bool mIsMultithreadedRendering;                 // 启用多线程渲染
		bool mIsVsync = true;							// 是否垂直同步 
		ViewPort mViewport;
		uint64_t mCurrentFrameCount = 0;

		struct EmptyRhiState {};
		std::shared_ptr<EmptyRhiState> mEmptyRhiState = nullptr;

		std::list<GPUResource*> mGPUResource;  // 记录所有注册的GPUResource
		std::vector<GPUResource*> mRemovedGPUResources;
	};
}