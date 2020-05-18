#pragma once

#include "renderer\RHI\rhiResource.h"

namespace Cjing3D
{
	// 着色器结构
	class Shader : public GraphicsDeviceChild
	{
	public:
		Shader(SHADERSTAGES stage) : mStage(stage) {};

		struct ShaderByteCode
		{
			BYTE* mByteData;
			size_t mByteLength;

			ShaderByteCode() : mByteData(nullptr), mByteLength(0) {};
			~ShaderByteCode() { SAFE_DELETE_ARRAY(mByteData); }
		};

		ShaderByteCode mByteCode;
		const SHADERSTAGES mStage = SHADERSTAGES_COUNT;
	};

	class VertexShader : public Shader { public: VertexShader() : Shader(SHADERSTAGES_VS) {} };
	class PixelShader : public Shader { public: PixelShader() : Shader(SHADERSTAGES_PS) {} };
	class ComputeShader : public Shader { public:ComputeShader() : Shader(SHADERSTAGES_CS) {}};
	class HullShader : public Shader { public: HullShader() : Shader(SHADERSTAGES_HS) {} };
	class DomainShader : public Shader { public: DomainShader() : Shader(SHADERSTAGES_DS) {} };

	using VertexShaderPtr = std::shared_ptr<VertexShader>;
	using PixelShaderPtr = std::shared_ptr<PixelShader>;
	using ComputeShaderPtr = std::shared_ptr<ComputeShader>;
	using HullShaderPtr = std::shared_ptr<HullShader>;
	using DomainShaderPtr = std::shared_ptr<DomainShader>;

	// 顶点着色器信息
	class VertexShaderInfo
	{
	public:
		std::shared_ptr<InputLayout> mInputLayout;
		std::shared_ptr<VertexShader> mVertexShader;
	};

	template <typename T>
	struct is_shader : public std::false_type {};
	template <>
	struct is_shader<VertexShader> : public std::true_type {};
	template <>
	struct is_shader<PixelShader> : public std::true_type {};
	template <>
	struct is_shader<ComputeShader> : public std::true_type {};
	template <>
	struct is_shader<HullShader> : public std::true_type {};
	template <>
	struct is_shader<DomainShader> : public std::true_type {};

	struct PipelineStateDesc
	{
		std::shared_ptr < VertexShader>      mVertexShader = nullptr;
		std::shared_ptr < PixelShader>       mPixelShader = nullptr;
		std::shared_ptr < HullShader>        mHullShader = nullptr;
		std::shared_ptr < DomainShader>      mDomainShader = nullptr;
		std::shared_ptr < DepthStencilState> mDepthStencilState = nullptr;
		std::shared_ptr < BlendState>	     mBlendState = nullptr;
		std::shared_ptr < RasterizerState>   mRasterizerState = nullptr;
		std::shared_ptr < InputLayout>       mInputLayout = nullptr;
		PRIMITIVE_TOPOLOGY       mPrimitiveTopology = UNDEFINED_TOPOLOGY;
	};

	// shader info state
	class PipelineState : public GraphicsDeviceChild
	{
	public:
		PipelineStateDesc mDesc;
		
	};
}