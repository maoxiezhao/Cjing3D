#pragma once

#include "renderer\renderableCommon.h"
#include "renderer\RHI\rhiResource.h"
#include "renderer\RHI\rhiShader.h"
#include "renderer\RHI\device.h"
#include "presetDefinitions.h"
#include "shaderDefinitions.h"

namespace Cjing3D
{
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

		// shader
		inline std::shared_ptr<InputLayout> GetVertexLayout(InputLayoutType layoutType) {
			return mInputLayout[(size_t)layoutType];
		}
		inline std::shared_ptr<Shader> GetVertexShader(VetextShaderType shaderType) {
			return mVertexShader[(size_t)shaderType];
		}
		inline std::shared_ptr<Shader> GetPixelShader(PixelShaderType shaderType) {
			return mPixelShader[(size_t)shaderType];
		}
		inline std::shared_ptr<Shader>  GetComputeShader(ComputeShaderType shaderType) {
			return mComputeShader[(size_t)shaderType];
		}

	private:
		void SetupDepthStencilStates();
		void SetupBlendStates();
		void SetupRasterizerStates();
		void SetupSamplerStates();
		void LoadConstantBuffers();
		void LoadStructuredBuffers();
		void LoadShaderPreset();

	private:
		GraphicsDevice& mDevice;

		// normal 
		std::shared_ptr<DepthStencilState> mDepthStencilStates[DepthStencilStateID_Count];
		std::shared_ptr<BlendState> mBlendStates[BlendStateID_Count];
		std::shared_ptr<RasterizerState> mRasterizerStates[RasterizerStateID_Count];
		std::shared_ptr<SamplerState> mSamplerStates[SamplerStateID_Count];

		GPUBuffer mConstantBuffer[ConstantBufferType_Count];
		GPUBuffer mStructuredBuffer[StructuredBufferType_Count];

		std::map<StringID, GPUBuffer> mCustomBufferMap;

		// shader
		std::shared_ptr<InputLayout> mInputLayout[InputLayoutType_Count];
		std::shared_ptr<Shader> mVertexShader[VertexShaderType_Count];
		std::shared_ptr<Shader> mPixelShader[PixelShaderType_Count];
		std::shared_ptr<Shader> mComputeShader[ComputeShaderType_Count];
	};
}