#pragma once

#include "renderer\renderableCommon.h"
#include "renderer\RHI\rhiStructure.h"
#include "renderer\includeD3D11.h"

namespace Cjing3D
{
	class DepthStencilState
	{
	public:
		DepthStencilState() : mState() {};

		DepthStencilStateDesc GetDesc()const { return mDesc; }
		ID3D11DepthStencilState& GetState() { return *mState.Get(); }
		ComPtr<ID3D11DepthStencilState>& GetStatePtr() { return mState; }
	private:
		DepthStencilStateDesc mDesc;
		ComPtr<ID3D11DepthStencilState> mState;
	};

	class BlendState
	{
	public:
		BlendState() : mState() {};

		BlendStateDesc GetDesc()const { return mDesc; }
		ID3D11BlendState& GetState() { return *mState.Get(); }
		ComPtr<ID3D11BlendState>& GetStatePtr() { return mState; }
	private:
		BlendStateDesc mDesc;
		ComPtr<ID3D11BlendState> mState;
	};

	class RasterizerState
	{
	public:
		RasterizerState() : mState() {};

		BlendStateDesc GetDesc()const { return mDesc; }
		ID3D11RasterizerState& GetState() { return *mState.Get(); }
		ComPtr<ID3D11RasterizerState>& GetStatePtr() { return mState; }
	private:
		BlendStateDesc mDesc;
		ComPtr<ID3D11RasterizerState> mState;
	};

	class InputLayout
	{
	public:
		InputLayout() {};
		~InputLayout() {};

	private:
		ComPtr<ID3D11InputLayout> mResourceD3D11;
		std::vector<VertexLayoutDesc> mDescs;
	};

	// 着色器结构
	template<typename D3D11ShaderType>
	class Shader
	{
	public:
		Shader() {}
		~Shader() {}

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

	// 顶点着色器信息
	class VertexShaderInfo
	{
		std::shared_ptr<InputLayout> mInputLayout;
		std::shared_ptr<VertexShader> mVertexShader;
	};
}