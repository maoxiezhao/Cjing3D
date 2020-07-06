#pragma once

#include "renderer\renderableCommon.h"
#include "renderer\renderer.h"
#include "renderer\RHI\rhiResource.h"
#include "renderer\RHI\rhiShader.h"

namespace Cjing3D
{
	enum StructuredBufferType
	{
		StructuredBufferType_ShaderLight = 0,
		StructuredBufferType_MatrixArray,
		StructuredBufferType_Count,
	};

	enum ConstantBufferType
	{
		ConstantBufferType_Common = 0,
		ConstantBufferType_Camera,
		ConstantBufferType_Frame,
		ConstantBufferType_Image,
		ConstantBufferType_Postprocess,
		ConstantBufferType_MipmapGenerate,
		ConstantBufferType_CubeMap,
		ConstantBufferType_CSParams,
		ConstantBufferType_Count,
	};

	// depthStencilState
	enum DepthStencilStateID {
		DepthStencilStateID_DepthNone = 0,
		DepthStencilStateID_GreaterEqualReadWrite,
		DepthStencilStateID_DepthRead,
		DepthStencilStateID_DepthReadEqual,
		DepthStencilStateID_Shadow,
		DepthStencilStateID_Count
	};

	// blend State
	enum BlendStateID {
		BlendStateID_Opaque = 0,
		BlendStateID_Transpranent,
		BlendStateID_PreMultiplied,
		BlendStateID_ColorWriteDisable,
		BlendStateID_Count
	};

	// Rasterizer State
	enum RasterizerStateID {
		RasterizerStateID_Front = 0,
		RasterizerStateID_Image,
		RasterizerStateID_Shadow,
		RasterizerStateID_WireFrame,
		RasterizerStateID_Sky,
		RasterizerStateID_Count
	};

	// Sampler State
	enum SamplerStateID {
		SamplerStateID_PointClampAlways,
		SamplerStateID_PointClampGreater,
		SamplerStateID_LinearClampGreater,
		SamplerStateID_LinearWrapGreater,
		SamplerStateID_ANISOTROPIC,
		SamplerStateID_Comparision_depth,
		SamplerStateID_Font,
		SamplerStateID_Count
	};

	class RenderPreset
	{
	public:
		RenderPreset(GraphicsDevice& device);
		~RenderPreset();

		void Initialize();
		void Uninitialize();

		inline std::shared_ptr<DepthStencilState> GetDepthStencilState(DepthStencilStateID id) {
			return mDepthStencilStates[(size_t)id];
		}
		inline std::shared_ptr<BlendState> GetBlendState(BlendStateID id) {
			return mBlendStates[(size_t)id];
		}
		inline std::shared_ptr<RasterizerState> GetRasterizerState(RasterizerStateID id) {
			return mRasterizerStates[(size_t)id];
		}
		inline std::shared_ptr<SamplerState> GetSamplerState(SamplerStateID id) {
			return mSamplerStates[(size_t)id];
		}
		inline GPUBuffer& GetConstantBuffer(ConstantBufferType bufferType) {
			return mConstantBuffer[static_cast<int>(bufferType)];
		}

		inline GPUBuffer& GetStructuredBuffer(StructuredBufferType bufferType) {
			return mStructuredBuffer[static_cast<int>(bufferType)];
		}

		GPUBuffer& GetOrCreateCustomBuffer(const StringID& name);

	private:
		void SetupDepthStencilStates();
		void SetupBlendStates();
		void SetupRasterizerStates();
		void SetupSamplerStates();
		void LoadConstantBuffers();
		void LoadStructuredBuffers();

	private:
		GraphicsDevice & mDevice;

		std::shared_ptr<DepthStencilState> mDepthStencilStates[DepthStencilStateID_Count];
		std::shared_ptr<BlendState> mBlendStates[BlendStateID_Count];
		std::shared_ptr<RasterizerState> mRasterizerStates[RasterizerStateID_Count];
		std::shared_ptr<SamplerState> mSamplerStates[SamplerStateID_Count];

		GPUBuffer mConstantBuffer[ConstantBufferType_Count];
		GPUBuffer mStructuredBuffer[StructuredBufferType_Count];

		std::map<StringID, GPUBuffer> mCustomBufferMap;

	};
}