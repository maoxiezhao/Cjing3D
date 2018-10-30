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
		Shader() : Resource(Resource::DeduceResourceType(Shader<D3D11ShaderType>)) {};
		~Shader() {};

	private:
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
}