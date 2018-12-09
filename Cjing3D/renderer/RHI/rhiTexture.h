#pragma once

#include "renderer\RHI\rhiResource.h"
#include "resource\resource.h"

namespace Cjing3D
{
	class GraphicsDevice;

	using CPUHandle = uint64_t;
	static const CPUHandle CPU_NULL_HANDLE = 0;
	class GPUResource
	{
	public:
		GPUResource(GraphicsDevice& device);
		virtual ~GPUResource();

		CPUHandle GetGPUResource() { return mResource; }
		CPUHandle GetShaderResourceView() { return mSRV; }
		GraphicsDevice& GetDevice() { return mDevice; }

	private:
		CPUHandle mResource = CPU_NULL_HANDLE;
		CPUHandle mSRV = CPU_NULL_HANDLE;
		GraphicsDevice& mDevice;
	};

	class RhiTexture : public Resource, public GPUResource
	{
	public:
		RhiTexture(GraphicsDevice& device);
		~RhiTexture();

		const TextureDesc& GetDesc()const { return mDesc; }

		ID3D11RenderTargetView& GetRenderTargetView() {
			return *mRTV.Get();
		}
		ComPtr<ID3D11RenderTargetView>& GetRenderTargetViewPtr() {
			return mRTV;
		}

	private:
		TextureDesc mDesc;
		ComPtr<ID3D11RenderTargetView> mRTV;
	};

	class RhiTexture2D : public RhiTexture
	{
	public:
		RhiTexture2D(GraphicsDevice& device);
		~RhiTexture2D();

		ID3D11DepthStencilView& GetDepthStencilView() {
			return *mDSV.Get();
		}
		ComPtr<ID3D11DepthStencilView>& GetDepthStencilViewPtr() {
			return mDSV;
		}

	private:
		ComPtr<ID3D11DepthStencilView> mDSV;
	};
}