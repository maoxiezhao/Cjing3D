#pragma once

#include "renderer\RHI\rhiStructure.h"
#include "renderer\includeD3D11.h"

namespace Cjing3D
{
	class GraphicsDevice;

	using CPUHandle = uint64_t;
	static const CPUHandle CPU_NULL_HANDLE = 0;

	class GraphicsDeviceChild
	{
	public:
		GraphicsDeviceChild() = default;

		inline bool IsValid()const { return mDevice != nullptr; }

		virtual void Register(GraphicsDevice* device) { mDevice = device; }
		virtual void UnRegister() { mDevice = nullptr; }

		GraphicsDevice * mDevice = nullptr;
	};

	//***********************************************************************
	// GPU Resources
	//***********************************************************************

	enum GPU_RESOURCE_TYPE
	{
		BUFFER,
		TEXTURE_1D,
		TEXTURE_2D,
		UNKNOWN_TYPE
	};

	class GPUResource : public GraphicsDeviceChild
	{
	public:
		GPUResource() = default;
		virtual ~GPUResource();

		virtual void Register(GraphicsDevice* device);
		virtual void UnRegister();

		CPUHandle& GetGPUResource() { return mResource; }
		CPUHandle& GetShaderResourceView() { return mSRV; }
		CPUHandle& GetUnorderedAccessView() { return mUAV; }
		CPUHandle& GetSubShaderResourceView(U32 index) { assert(index < mSubresourceSRVs.size()); return mSubresourceSRVs[index]; }
		CPUHandle& GetSubUnorderedAccessView(U32 index) { assert(index < mSubresourceUAVS.size()); return mSubresourceUAVS[index]; }

		inline bool IsBuffer()const { return mCurrType == GPU_RESOURCE_TYPE::BUFFER; }
		inline bool IsTexture()const { return mCurrType == GPU_RESOURCE_TYPE::TEXTURE_1D || mCurrType == GPU_RESOURCE_TYPE::TEXTURE_2D; }

		GPU_RESOURCE_TYPE mCurrType = GPU_RESOURCE_TYPE::UNKNOWN_TYPE;
		CPUHandle mResource = CPU_NULL_HANDLE;

		CPUHandle mSRV = CPU_NULL_HANDLE;
		CPUHandle mUAV = CPU_NULL_HANDLE;

		std::vector<CPUHandle> mSubresourceSRVs;
		std::vector<CPUHandle> mSubresourceUAVS;
	};

	class GPUBuffer : public GPUResource
	{
	public:
		GPUBufferDesc mDesc;

		ID3D11Buffer& GetBuffer() { return **((ID3D11Buffer**)&GetGPUResource());}
		GPUBufferDesc GetDesc() { return mDesc; }
		void SetDesc(GPUBufferDesc desc) {	mDesc = desc;}
	};

	class RhiTexture : public GPUResource
	{
	public:
		TextureDesc mDesc;
		CPUHandle mRTV = CPU_NULL_HANDLE;

		const TextureDesc& GetDesc()const { return mDesc; }
		void SetDesc(TextureDesc& desc) { mDesc = desc; }

		ID3D11RenderTargetView* GetRenderTargetView() {return ((ID3D11RenderTargetView*)mRTV);}
		ID3D11RenderTargetView** GetRenderTargetViewPtr() { return ((ID3D11RenderTargetView**)&mRTV); }
	};

	class RhiTexture2D : public RhiTexture
	{
	public:
		CPUHandle mDSV = CPU_NULL_HANDLE;
		~RhiTexture2D();

		virtual void UnRegister();

		ID3D11DepthStencilView* GetDepthStencilView() { return *((ID3D11DepthStencilView**)&mDSV); }
		ID3D11DepthStencilView** GetDepthStencilViewPtr() { return ((ID3D11DepthStencilView**)&mDSV); }
	};
	using RhiTexture2DPtr = std::shared_ptr<RhiTexture2D>;

	using Texture1D = RhiTexture;
	using Texture2D = RhiTexture2D;
	using Texture2DPtr = RhiTexture2DPtr;

	//***********************************************************************
	// GPU States
	//***********************************************************************

	class RasterizerState : public GraphicsDeviceChild
	{
	public:
		RasterizerState() {};

		void SetDesc(RasterizerStateDesc desc) { mDesc = desc; }
		RasterizerStateDesc GetDesc()const { return mDesc; }
		ID3D11RasterizerState* GetRasterizerState() { return *(ID3D11RasterizerState**)&mHandle; }
		ID3D11RasterizerState** GetRasterizerStatePtr() { return (ID3D11RasterizerState**)&mHandle; }

		RasterizerStateDesc mDesc;
		CPUHandle mHandle = CPU_NULL_HANDLE;
	};

	class DepthStencilState : public GraphicsDeviceChild
	{
	public:
		DepthStencilState() {};

		void SetDesc(DepthStencilStateDesc desc) { mDesc = desc; }
		DepthStencilStateDesc GetDesc()const { return mDesc; }
		ID3D11DepthStencilState* GetDepthStencilState() { return *(ID3D11DepthStencilState**)&mHandle; }
		ID3D11DepthStencilState** GetDepthStencilStatePtr() { return (ID3D11DepthStencilState**)&mHandle; }

		DepthStencilStateDesc mDesc;
		CPUHandle mHandle = CPU_NULL_HANDLE;
	};

	class BlendState : public GraphicsDeviceChild
	{
	public:
		BlendState() {};

		void SetDesc(BlendStateDesc desc) { mDesc = desc; }
		BlendStateDesc GetDesc()const { return mDesc; }
		ID3D11BlendState* GetBlendState() { return *(ID3D11BlendState**)&mHandle; }
		ID3D11BlendState** GetBlendStatePtr() { return (ID3D11BlendState**)&mHandle; }

		BlendStateDesc mDesc;
		CPUHandle mHandle = CPU_NULL_HANDLE;
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
}