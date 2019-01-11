#pragma once

#include "renderer\RHI\rhiResource.h"
#include "resource\resource.h"

namespace Cjing3D
{
	class GraphicsDevice;

	class RhiTexture : public GPUResource
	{
	public:
		RhiTexture(GraphicsDevice& device);
		~RhiTexture();

		const TextureDesc& GetDesc()const { return mDesc; }
		void SetDesc(TextureDesc& desc) { mDesc = desc; }

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

	using RhiTexture2DPtr = std::shared_ptr<RhiTexture2D>;
}