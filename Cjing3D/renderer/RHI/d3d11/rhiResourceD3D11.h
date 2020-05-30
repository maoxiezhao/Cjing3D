#pragma once

#include "renderer\includeD3D11.h"

namespace Cjing3D
{
	class GPUResourceD3D11
	{
	public:
		ComPtr<ID3D11Resource> mResource = nullptr;
		ComPtr<ID3D11ShaderResourceView> mSRV = nullptr;
		ComPtr<ID3D11UnorderedAccessView> mUAV = nullptr;

		// 用于存储各个mipmap的SRV
		std::vector<ComPtr<ID3D11ShaderResourceView>> mSubresourceSRVs;	
		// 用于存储各个mipmap的UAV
		std::vector<ComPtr<ID3D11UnorderedAccessView>> mSubresourceUAVS;   
	};

	class TextureD3D11 : public GPUResourceD3D11
	{
	public:
		ComPtr<ID3D11RenderTargetView> mRTV = nullptr;
		ComPtr<ID3D11DepthStencilView> mDSV = nullptr;

		std::vector<ComPtr<ID3D11RenderTargetView>> mSubresourceRTVs;
		std::vector<ComPtr<ID3D11DepthStencilView>> mSubresourceDSVs;
	};

	class RasterizerStateD3D11
	{
	public:
		ComPtr<ID3D11RasterizerState> mHandle = nullptr;
	};

	class DepthStencilStateD3D11
	{
	public:
		ComPtr<ID3D11DepthStencilState> mHandle = nullptr;
	};

	class BlendStateD3D11
	{
	public:
		ComPtr<ID3D11BlendState> mHandle = nullptr;
	};

	class SamplerStateD3D11
	{
	public:
		ComPtr<ID3D11SamplerState> mHandle = nullptr;
	};

	class InputLayoutD3D11
	{
	public:
		ComPtr<ID3D11InputLayout> mHandle = nullptr;
	};

	class VertexShaderD3D11
	{
	public:
		ComPtr<ID3D11VertexShader> mHandle = nullptr;
	};

	class PixelShaderD3D11
	{
	public:
		ComPtr<ID3D11PixelShader> mHandle = nullptr;
	};

	class ComputeShaderD3D11
	{
	public:
		ComPtr<ID3D11ComputeShader> mHandle = nullptr;
	};

	class HullShaderD3D11
	{
	public:
		ComPtr<ID3D11HullShader> mHandle = nullptr;
	};

	class DomainShaderD3D11
	{
	public:
		ComPtr<ID3D11DomainShader> mHandle = nullptr;
	};

	class QueryD3D11
	{
	public:
		ComPtr<ID3D11Query> mHandle = nullptr;
	};
}