#pragma once

#include "renderer\renderableCommon.h"
#include "renderer\RHI\rhiResource.h"
#include "renderer\RHI\rhiDefinition.h"
#include "renderer\RHI\rhiStructure.h"
#include "renderer\RHI\rhiShader.h"
#include "renderer\RHI\rhiBuffer.h"

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

		virtual HRESULT CreateBuffer(const GPUBufferDesc* desc, ConstantBuffer& buffer) = 0;
		virtual void UpdateBuffer(ConstantBuffer& buffer, const void* data, U32 dataSize) = 0;

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

	protected:
		bool mIsFullScreen;
		FORMAT mBackBufferFormat;
		U32x2 mScreenSize;
		bool mIsMultithreadedRendering;
		bool mIsVsync;						/** 是否垂直同步 */
		ViewPort mViewport;
	};
}