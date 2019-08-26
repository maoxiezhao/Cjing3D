#pragma once

#include "renderer\RHI\rhiResource.h"
#include "resource\resource.h"

namespace Cjing3D
{
	// 着色器结构
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

	using VertexShaderPtr = std::shared_ptr<VertexShader>;
	using PixelShaderPtr = std::shared_ptr<PixelShader>;

	// 顶点着色器信息
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

	class GraphicsDeviceChild
	{
	public:
		GraphicsDevice* mDevice = nullptr;
		void RegisterGraphicsDevice(GraphicsDevice* device) { mDevice = device; }
	};


	// shader info state
	class ShaderInfoState : public GraphicsDeviceChild
	{
	public:
		VertexShaderPtr mVertexShader = nullptr;
		InputLayoutPtr mInputLayout = nullptr;
		PixelShaderPtr mPixelShader = nullptr;
		PRIMITIVE_TOPOLOGY mPrimitiveTopology = UNDEFINED_TOPOLOGY;

		void Clear() 
		{
			mVertexShader = nullptr;
			mInputLayout = nullptr;
			mPixelShader = nullptr;
		}

		bool IsEmpty()
		{
			return mVertexShader == nullptr ||
				mPixelShader == nullptr ||
				mInputLayout == nullptr;
		}
	};
}