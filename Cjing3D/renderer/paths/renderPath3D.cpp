#include "renderPath3D.h"
#include "renderer\renderer.h"
#include "renderImage.h"
#include "renderer\textureHelper.h"

namespace Cjing3D
{
	RenderPath3D::RenderPath3D(Renderer & renderer):
		RenderPath2D(renderer)
	{
	}

	RenderPath3D::~RenderPath3D()
	{
	}

	void RenderPath3D::Update(F32 dt)
	{
		RenderPath2D::Update(dt);

		mRenderer.UpdatFrameData(dt);
	}

	void RenderPath3D::ResizeBuffers()
	{
		RenderPath2D::ResizeBuffers();

		FORMAT backBufferFormat = mRenderer.GetDevice().GetBackBufferFormat();
		const auto screenSize = mRenderer.GetDevice().GetScreenSize();

		// depth buffer
		{
			TextureDesc desc = {};
			desc.mWidth = screenSize[0];
			desc.mHeight = screenSize[1];
			desc.mFormat = RenderPath3D::DepthStencilFormatAlias;
			desc.mBindFlags = BIND_DEPTH_STENCIL | BIND_SHADER_RESOURCE;

			const auto result = mRenderer.GetDevice().CreateTexture2D(&desc, nullptr, mDepthBuffer);
			Debug::ThrowIfFailed(result, "Failed to create depth buffer:%08x", result);
		}

		// post process render target
		{
			TextureDesc desc = {};
			desc.mWidth = screenSize[0];
			desc.mHeight = screenSize[1];
			desc.mFormat = backBufferFormat;
			desc.mBindFlags = BIND_RENDER_TARGET | BIND_SHADER_RESOURCE | BIND_UNORDERED_ACCESS;

			HRESULT result = mRenderer.GetDevice().CreateTexture2D(&desc, nullptr, mRTPostprocessLDR1);
			Debug::ThrowIfFailed(result, "Failed to create postprocess render target:%08x", result);
			mRenderer.GetDevice().SetResourceName(mRTPostprocessLDR1, "RTPostprocessLDR1");

			result = mRenderer.GetDevice().CreateTexture2D(&desc, nullptr, mRTPostprocessLDR2);
			Debug::ThrowIfFailed(result, "Failed to create postprocess render target:%08x", result);
			mRenderer.GetDevice().SetResourceName(mRTPostprocessLDR2, "RTPostprocessLDR2");
		}
	}

	void RenderPath3D::Compose()
	{
		GraphicsDevice& device = mRenderer.GetDevice();
		device.BeginEvent("Composition");

		RenderImage::ImageParams params;
		params.EnableFullScreen();
		params.mBlendType = BlendType_Opaque;

		RenderImage::Render(*GetLastPostprocessRT(), params, mRenderer);

		RenderPath2D::Compose();

		device.EndEvent();
	}

	void RenderPath3D::RenderTransparents(Texture2D& rtMain, RenderPassType renderType)
	{
		// render transparent
	}

	void RenderPath3D::RenderPostprocess(Texture2D & rtScreen)
	{
		Texture2D* rtRead = &rtScreen;
		Texture2D* rtWrite = GetLastPostprocessRT();

		// HDR-> LDR
		mRenderer.PostprocessTonemap(*rtRead, *rtWrite, GetExposure());

		// FXAA
		if (IsEnableFXAA())
		{
			rtRead = &mRTPostprocessLDR2;
			rtWrite = &mRTPostprocessLDR1;

			mRenderer.PostprocessFXAA(*rtRead, *rtWrite);

			TextureHelper::SwapTexture(*rtRead, *rtWrite);
		}
	}
}