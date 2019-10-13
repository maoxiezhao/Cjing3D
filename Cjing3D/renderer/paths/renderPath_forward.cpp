#include "renderPath_forward.h"
#include "renderer\renderer.h"
#include "renderer\paths\renderImage.h"

namespace Cjing3D {

	RenderPathForward::RenderPathForward(Renderer& renderer) :
		RenderPath3D(renderer)
	{
	}

	RenderPathForward::~RenderPathForward()
	{
	}

	void RenderPathForward::ResizeBuffers()
	{
		RenderPath3D::ResizeBuffers();

		const auto screenSize = mRenderer.GetDevice().GetScreenSize();
		TextureDesc desc = {};
		desc.mWidth = screenSize[0];
		desc.mHeight = screenSize[1];
		desc.mFormat = RenderPath3D::RenderTargetFormatHDR;
		desc.mBindFlags = BIND_RENDER_TARGET | BIND_SHADER_RESOURCE;
		{
			const auto result = mRenderer.GetDevice().CreateTexture2D(&desc, nullptr, mRTMain);
			Debug::ThrowIfFailed(result, "Failed to create render target:%08x", result);
		}
		{
			const auto result = mRenderer.GetDevice().CreateTexture2D(&desc, nullptr, mRTFinal);
			Debug::ThrowIfFailed(result, "Failed to create render target:%08x", result);
		}
	}

	void RenderPathForward::Render()
	{
		GraphicsDevice& device = mRenderer.GetDevice();
		auto camera = mRenderer.GetCamera();
		if (camera == nullptr) {
			return;
		}

		mRenderer.SetupRenderFrame();
		mRenderer.UpdateCameraCB(*camera);

		// opaque scene
		Texture2D& depthBuffer = GetDepthBuffer();
		Texture2D* rts[] = { &mRTMain };
		{
			device.BindRenderTarget(1, rts, &depthBuffer);
			device.ClearRenderTarget(mRTMain, { 0.0f, 0.0f, 0.0f, 0.0f });

			ViewPort vp;
			vp.mWidth = (F32)mRTMain.GetDesc().mWidth;
			vp.mWidth = (F32)mRTMain.GetDesc().mHeight;
			device.BindViewports(&vp, 1, GraphicsThread::GraphicsThread_IMMEDIATE);

			mRenderer.RenderSceneOpaque(camera, ShaderType_Forward);
		}

		// postprocess
		RenderPostprocess(mRTFinal);

		RenderPath2D::Render();
	}
}