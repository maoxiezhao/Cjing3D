#include "renderPath3D.h"
#include "renderer\renderer.h"
#include "renderImage.h"

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

			const auto result = mRenderer.GetDevice().CreateTexture2D(&desc, nullptr, mRTPostprocess);
			Debug::ThrowIfFailed(result, "Failed to create postprocess render target:%08x", result);
		}
	}

	void RenderPath3D::Compose()
	{
		RenderImage::ImageParams params;
		params.EnableFullScreen();
		params.mBlendType = BlendType_Opaque;

		RenderImage::Render(*GetLastPostprocessRT(), params, mRenderer);

		RenderPath2D::Compose();
	}

	void RenderPath3D::RenderPostprocess(Texture2D & rtScreen)
	{
		Texture2D* rtRead = &rtScreen;
		Texture2D* rtWrite = GetLastPostprocessRT();

		// do nothing now
	}
}