#pragma once

#include "renderer\RHI\rhiResource.h"

namespace Cjing3D
{
	// 着色器结构
	class Shader : public GraphicsDeviceChild
	{
	public:
		Shader() {};
		Shader(SHADERSTAGES stage) : mStage(stage) {};

		inline bool IsValid()const { return mStage != SHADERSTAGES_COUNT && GraphicsDeviceChild::IsValid(); }

		struct ShaderByteCode
		{
			BYTE* mByteData;
			size_t mByteLength;

			ShaderByteCode() : mByteData(nullptr), mByteLength(0) {};
			~ShaderByteCode() { SAFE_DELETE_ARRAY(mByteData); }
		};

		ShaderByteCode mByteCode;
		SHADERSTAGES mStage = SHADERSTAGES_COUNT;
	};
	using ShaderPtr = std::shared_ptr<Shader>;

	// 顶点着色器信息
	class VertexShaderInfo
	{
	public:
		InputLayoutPtr mInputLayout = nullptr;
		ShaderPtr mVertexShader = nullptr;
	};

	template <typename T>
	struct is_shader : public std::false_type {};

	struct PipelineStateDesc
	{
		std::shared_ptr<Shader>			   mVertexShader = nullptr;
		std::shared_ptr<Shader>			   mPixelShader = nullptr;
		std::shared_ptr<Shader>            mHullShader = nullptr;
		std::shared_ptr<Shader>            mDomainShader = nullptr;
		std::shared_ptr<DepthStencilState> mDepthStencilState = nullptr;
		std::shared_ptr<BlendState>	       mBlendState = nullptr;
		std::shared_ptr<RasterizerState>   mRasterizerState = nullptr;
		std::shared_ptr<InputLayout>       mInputLayout = nullptr;
		PRIMITIVE_TOPOLOGY			       mPrimitiveTopology = UNDEFINED_TOPOLOGY;

		inline void Clear()
		{
			mVertexShader = nullptr;
			mPixelShader = nullptr;
			mHullShader = nullptr;
			mDomainShader = nullptr;
			mDepthStencilState = nullptr;
			mBlendState = nullptr;
			mRasterizerState = nullptr;
			mInputLayout = nullptr;
		}
	};

	// shader info state
	class PipelineState : public GraphicsDeviceChild
	{
	public:
		PipelineStateDesc mDesc;
		
		inline void Clear() { mDesc.Clear(); GraphicsDeviceChild::Clear(); }
	};
}