#pragma once

#include "renderer\RHI\rhiStructure.h"
#include "renderer\includeD3D11.h"

namespace Cjing3D
{
	class GraphicsDevice;

	using CPUHandle = uint64_t;
	static const CPUHandle CPU_NULL_HANDLE = 0;
	class GPUResource
	{
	public:
		GPUResource(GraphicsDevice& device) :mDevice(device) {};
		virtual ~GPUResource();

		CPUHandle& GetGPUResource() { return mResource; }
		CPUHandle& GetShaderResourceView() { return mSRV; }
		GraphicsDevice& GetDevice() { return mDevice; }

	private:
		CPUHandle mResource = CPU_NULL_HANDLE;
		CPUHandle mSRV = CPU_NULL_HANDLE;
		GraphicsDevice& mDevice;
	};

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

	using InputLayoutPtr = std::shared_ptr<InputLayout>;

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