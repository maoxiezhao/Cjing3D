#pragma once

#include "renderer\renderableCommon.h"
#include "renderer\RHI\rhiResource.h"
#include "renderer\RHI\rhiDefinition.h"
#include "renderer\RHI\rhiStructure.h"
#include "renderer\RHI\rhiShader.h"
#include "renderer\RHI\rhiBuffer.h"
#include "renderer\RHI\rhiTexture.h"

namespace Cjing3D
{
	class GraphicsDevice
	{
	public:
		GraphicsDevice();

		virtual void Initialize();
		virtual void Uninitialize();

		virtual void PresentBegin() = 0;
		virtual void PresentEnd() = 0;

		virtual void BindViewports(const ViewPort* viewports, U32 numViewports, GraphicsThread threadID) = 0;

		virtual HRESULT CreateDepthStencilState(const DepthStencilStateDesc& desc, DepthStencilState& state) = 0;
		virtual HRESULT CreateBlendState(const BlendStateDesc& desc, BlendState& state) = 0;
		virtual HRESULT CreateRasterizerState(const RasterizerStateDesc& desc, RasterizerState& state) = 0;

		virtual HRESULT CreateVertexShader(const void* bytecode, size_t length, VertexShader& vertexShader) = 0;
		virtual HRESULT CreateInputLayout(VertexLayoutDesc* desc, U32 numElements, const void* shaderBytecode, size_t shaderLength, InputLayout& inputLayout) = 0;

		virtual HRESULT CreateBuffer(const GPUBufferDesc* desc, GPUBuffer& buffer, const SubresourceData* initialData) = 0;
		virtual void UpdateBuffer(GPUBuffer& buffer, const void* data, U32 dataSize) = 0;

		virtual HRESULT BindIndexBuffer(GPUBuffer& buffer, IndexFormat format, U32 offset) = 0;
		virtual HRESULT BindVertexBuffer(GPUBuffer* const* buffer, U32 slot, U32 num, const U32* strides, const U32* offsets) = 0;

		virtual HRESULT CreateSamplerState(const SamplerDesc* desc, SamplerState& state) = 0;

		virtual HRESULT CreateTexture2D(const TextureDesc* desc, const SubresourceData* data, RhiTexture2D& texture2D) = 0;

		virtual void BindRenderTarget(UINT numView, RhiTexture2D* const *texture2D, RhiTexture2D* depthStencilTexture) = 0;

		virtual HRESULT CreateRenderTargetView(RhiTexture2D& texture) = 0;
		virtual HRESULT CreateShaderResourceView(RhiTexture2D& texture) = 0;
		virtual HRESULT CreateDepthStencilView(RhiTexture2D& texture) = 0;

		virtual void ClearRenderTarget(RhiTexture2D& texture, F32x4 color) = 0;
		virtual void ClearDepthStencil(RhiTexture2D& texture, UINT clearFlag, F32 depth, U8 stencil) = 0;

		virtual void BindGPUResource(SHADERSTAGES stage, GPUResource& resource, U32 slot) = 0;
		virtual void BindGPUResources(SHADERSTAGES stage, GPUResource* const* resource, U32 slot, U32 count) = 0;
		virtual void DestoryGPUResource(GPUResource& resource) = 0;

		virtual void BindShaderInfoState(ShaderInfoState* state) = 0;

		virtual void DrawIndexed() = 0;
		virtual void DrawIndexedInstances(U32 indexCount, U32 instanceCount, U32 startIndexLocation, U32 baseVertexLocation, U32 startInstanceLocation) = 0;

		FORMAT GetBackBufferFormat()const {
			return mBackBufferFormat;
		}

		U32x2 GetScreenSize()const {
			return mScreenSize;
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

	protected:
		bool mIsFullScreen;
		FORMAT mBackBufferFormat;
		U32x2 mScreenSize;
		bool mIsMultithreadedRendering;
		bool mIsVsync;						/** �Ƿ�ֱͬ�� */
		ViewPort mViewport;
		uint64_t mCurrentFrameCount = 0;

		struct GPUAllocator
		{
			GPUBuffer buffer;
			size_t byteOffset = 0;
			uint64_t residentFrame = 0;
			bool dirty = false;

			size_t GetDataSize(){ return buffer.GetDesc().mByteWidth; }
		};
		GPUAllocator mGPUAllocator;
	};
}