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
		desc.mSampleDesc.mCount = 1;
		desc.mBindFlags = BIND_RENDER_TARGET | BIND_SHADER_RESOURCE | BIND_UNORDERED_ACCESS;
		{
			const auto result = mRenderer.GetDevice().CreateTexture2D(&desc, nullptr, mRTMain);
			Debug::ThrowIfFailed(result, "Failed to create render target:%08x", result);

			mRenderer.GetDevice().SetResourceName(mRTMain, "RTMain");
		}
	}

	void RenderPathForward::Render()
	{
		GraphicsDevice& device = mRenderer.GetDevice();
		auto camera = mRenderer.GetCamera();
		if (camera == nullptr) {
			return;
		}

		mRenderer.UpdateCameraCB(*camera);

		// opaque scene
		Texture2D& depthBuffer = *GetDepthBuffer();
		Texture2D* rts[] = { &mRTMain };
		{
			device.BindRenderTarget(1, rts, &depthBuffer);
			device.ClearRenderTarget(mRTMain, { 0.0f, 0.0f, 0.0f, 1.0f });
			device.ClearDepthStencil(depthBuffer, CLEAR_DEPTH | CLEAR_STENCIL, 0, 0);

			ViewPort vp;
			vp.mWidth = (F32)mRTMain.GetDesc().mWidth;
			vp.mHeight = (F32)mRTMain.GetDesc().mHeight;
			vp.mMinDepth = 0.0f;
			vp.mMaxDepth = 1.0f;
			device.BindViewports(&vp, 1, GraphicsThread::GraphicsThread_IMMEDIATE);

			mRenderer.RenderSceneOpaque(camera, ShaderType_Forward);
		}

		mRenderer.BindCommonResource();

		// postprocess
		RenderPostprocess(mRTMain);

		RenderPath2D::Render();
	}

	void RenderPathForward::Compose()
	{
		GraphicsDevice& device = mRenderer.GetDevice();
		device.BeginEvent("Composition");

		RenderImage::ImageParams params;
		params.mBlendType = BlendType_Opaque;

		RenderImage::Render(mRTMain, params, mRenderer);

		RenderPath2D::Compose();

		device.EndEvent();
	}
}