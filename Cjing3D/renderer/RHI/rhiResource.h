#pragma once

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

		const std::vector<VertexLayoutDesc>& GetDesc()const { return mDescs; }
		ID3D11InputLayout& GetState() { return *mResourceD3D11.Get(); }
		ComPtr<ID3D11InputLayout>& GetStatePtr() { return mResourceD3D11; }
	private:
		ComPtr<ID3D11InputLayout> mResourceD3D11;
		std::vector<VertexLayoutDesc> mDescs;
	};

	class SamplerState
	{
	public:
		SamplerState() : mState() {};

		SamplerDesc GetDesc()const { return mDesc; }
		ID3D11SamplerState& GetState() { return *mState.Get(); }
		ComPtr<ID3D11SamplerState>& GetStatePtr() { return mState; }
	private:
		SamplerDesc mDesc;
		ComPtr<ID3D11SamplerState> mState;
	};
}