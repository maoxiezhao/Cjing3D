#include "renderPath2D.h"
#include "renderer\renderer.h"
#include "renderer\paths\renderImage.h"

namespace Cjing3D
{
	RenderPath2D::RenderPath2D(Renderer & renderer):
		RenderPath(renderer)
	{
	}

	RenderPath2D::~RenderPath2D()
	{
	}

	void RenderPath2D::ResizeBuffers()
	{
		RenderPath::ResizeBuffers();

		const auto screenSize = mRenderer.GetDevice().GetScreenSize();
		FORMAT format = mRenderer.GetDevice().GetBackBufferFormat();

		TextureDesc desc = {};
		desc.mWidth = screenSize[0];
		desc.mHeight = screenSize[1];
		desc.mFormat = format;
		desc.mBindFlags = BIND_RENDER_TARGET | BIND_SHADER_RESOURCE;
		{
			const auto result = mRenderer.GetDevice().CreateTexture2D(&desc, nullptr, mRTFinal);
			Debug::ThrowIfFailed(result, "Failed to create render target:%08x", result);

			mRenderer.GetDevice().SetResourceName(mRTFinal, "mRTFinal");
		}
	}

	void RenderPath2D::Initialize()
	{
		RenderPath::Initialize();
	}

	void RenderPath2D::Uninitialize()
	{
		RenderPath::Uninitialize();
	}

	void RenderPath2D::Update(F32 dt)
	{
		RenderPath::Update(dt);
	}

	void RenderPath2D::Render()
	{
		GraphicsDevice& device = mRenderer.GetDevice();
		device.BeginEvent("Render2D");

		Texture2D* depthBuffer = GetDepthBuffer();
		Texture2D* rts[] = { &mRTFinal };
		{
			device.BindRenderTarget(1, rts, depthBuffer);
			device.ClearRenderTarget(mRTFinal, { 0.0f, 0.0f, 0.0f, 0.0f });

			ViewPort vp;
			vp.mWidth = (F32)mRTFinal.GetDesc().mWidth;
			vp.mHeight = (F32)mRTFinal.GetDesc().mHeight;
			vp.mMinDepth = 0.0f;
			vp.mMaxDepth = 1.0f;
			device.BindViewports(&vp, 1, GraphicsThread::GraphicsThread_IMMEDIATE);

			mGUIRenderer.Render();
		}

		device.EndEvent();

		RenderPath::Render();
	}

	void RenderPath2D::Compose()
	{
		RenderImage::ImageParams params;
		params.mBlendType = BlendType_PreMultiplied;

		RenderImage::Render(mRTFinal, params, mRenderer);

		RenderPath::Compose();
	}
}