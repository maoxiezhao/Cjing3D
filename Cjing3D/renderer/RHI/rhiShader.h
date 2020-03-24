#pragma once

#include "renderer\RHI\rhiResource.h"
#include "resource\resource.h"

namespace Cjing3D
{
	// ��ɫ���ṹ
	template<typename D3D11ShaderType>
	class Shader : public Resource
	{
	public:
		Shader() : Resource(Resource::DeduceResourceType<Shader<D3D11ShaderType>>()) {};
		~Shader() {};

		struct ShaderByteCode
		{
			BYTE* mByteData;
			size_t mByteLength;

			ShaderByteCode() : mByteData(nullptr), mByteLength(0) {};
			~ShaderByteCode() { SAFE_DELETE_ARRAY(mByteData); }
		};

		ShaderByteCode mByteCode;
		ComPtr<D3D11ShaderType> mResourceD3D11;
	};

	using VertexShader = Shader<ID3D11VertexShader>;
	using PixelShader = Shader<ID3D11PixelShader>;
	using ComputeShader = Shader<ID3D11ComputeShader>;
	using HullShader = Shader<ID3D11HullShader>;
	using DomainShader = Shader<ID3D11DomainShader>;

	using VertexShaderPtr = std::shared_ptr<VertexShader>;
	using PixelShaderPtr = std::shared_ptr<PixelShader>;
	using ComputeShaderPtr = std::shared_ptr<ComputeShader>;
	using HullShaderPtr = std::shared_ptr<HullShader>;
	using DomainShaderPtr = std::shared_ptr<DomainShader>;

	// ������ɫ����Ϣ
	class VertexShaderInfo : public Resource
	{
	public:
		VertexShaderInfo() : Resource(Resrouce_VertexShader) {}

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

	// shader info state
	class PipelineState : public GraphicsDeviceChild
	{
	public:
		VertexShaderPtr mVertexShader = nullptr;
		PixelShaderPtr mPixelShader = nullptr;
		HullShaderPtr mHullShader = nullptr;
		DomainShaderPtr mDomainShader = nullptr;

		std::shared_ptr<DepthStencilState> mDepthStencilState = nullptr;
		std::shared_ptr<BlendState> mBlendState = nullptr;
		std::shared_ptr<RasterizerState> mRasterizerState = nullptr;

		InputLayoutPtr mInputLayout = nullptr;
		PRIMITIVE_TOPOLOGY mPrimitiveTopology = UNDEFINED_TOPOLOGY;

		void Clear() 
		{
			mVertexShader = nullptr;
			mInputLayout = nullptr;
			mPixelShader = nullptr;
			mHullShader = nullptr;
			mDomainShader = nullptr;

			mDepthStencilState = nullptr;
			mBlendState = nullptr;
			mRasterizerState = nullptr;
			mPrimitiveTopology = UNDEFINED_TOPOLOGY;
		}

		bool IsEmpty()
		{
			return false;
			//return mVertexShader == nullptr ||
			//	mPixelShader == nullptr ||
			//	mInputLayout == nullptr ||
			//	mBlendState == nullptr  ||
			//	mDepthStencilState == nullptr ||
			//	mRasterizerState == nullptr;
		}

		bool operator== (const PipelineState& other)
		{
			return (
				mVertexShader == other.mVertexShader &&
				mPixelShader == other.mPixelShader &&
				mHullShader == other.mHullShader &&
				mDomainShader == other.mDomainShader &&
				mInputLayout == other.mInputLayout &&
				mBlendState == other.mBlendState &&
				mDepthStencilState == other.mDepthStencilState &&
				mRasterizerState == other.mRasterizerState &&
				mPrimitiveTopology == other.mPrimitiveTopology
			);
		}
	};
}