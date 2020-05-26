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
		{
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
		//////////////////////////////////////////////////////////////////////////////////////////
		{
			RenderBehaviorDesc desc = {};
			desc.mParams.push_back({ RenderBehaviorParam::RenderType_DepthStencil, GetDepthBuffer(), -1, RenderBehaviorParam::RenderOperation_Clear });

			mRenderer.GetDevice().CreateRenderBehavior(desc, mRBDepthPrepass);
		}
		{
			RenderBehaviorDesc desc = {};
			desc.mParams.push_back({ RenderBehaviorParam::RenderType_RenderTarget, &mRTMain,         -1, RenderBehaviorParam::RenderOperation_Clear });
			desc.mParams.push_back({ RenderBehaviorParam::RenderType_DepthStencil, GetDepthBuffer(), -1, RenderBehaviorParam::RenderOperation_Load });

			mRenderer.GetDevice().CreateRenderBehavior(desc, mRBMain);
		}
		{
			RenderBehaviorDesc desc = {};
			desc.mParams.push_back({ RenderBehaviorParam::RenderType_RenderTarget, &mRTMain,         -1, RenderBehaviorParam::RenderOperation_Load });
			desc.mParams.push_back({ RenderBehaviorParam::RenderType_DepthStencil, GetDepthBuffer(), -1, RenderBehaviorParam::RenderOperation_Load });

			mRenderer.GetDevice().CreateRenderBehavior(desc, mRBTransparent);
		}
	}

	void RenderPathForward::Render()
	{
		GraphicsDevice& device = mRenderer.GetDevice();
		CameraComponent& camera = mRenderer.GetCamera();

		// update and bind common resources
		mRenderer.RefreshRenderData();

		// bind constant buffer
		mRenderer.BindCommonResource();
		mRenderer.BindConstanceBuffer(SHADERSTAGES_VS);
		mRenderer.BindConstanceBuffer(SHADERSTAGES_PS);
		
		// shadowmaps
		RenderShadowmaps();

		// update main camera constant buffer
		mRenderer.UpdateCameraCB(camera);

		// depth prepass
		{
			device.BeginEvent("depthPrepass");
			device.BeginRenderBehavior(mRBDepthPrepass);

			ViewPort vp;
			vp.mWidth = (F32)mDepthBuffer.GetDesc().mWidth;
			vp.mHeight = (F32)mDepthBuffer.GetDesc().mHeight;
			device.BindViewports(&vp, 1, GraphicsThread::GraphicsThread_IMMEDIATE);

			mRenderer.RenderSceneOpaque(camera, RenderPassType_Depth);

			device.EndRenderBehavior();
			device.EndEvent();
		}
		// opaque scene
		{
			device.BeginEvent("RenderSceneOpaque");
			device.BeginRenderBehavior(mRBMain);

			ViewPort vp;
			vp.mWidth = (F32)mRTMain.GetDesc().mWidth;
			vp.mHeight = (F32)mRTMain.GetDesc().mHeight;
			vp.mMinDepth = 0.0f;
			vp.mMaxDepth = 1.0f;
			device.BindViewports(&vp, 1, GraphicsThread::GraphicsThread_IMMEDIATE);

			mRenderer.RenderSceneOpaque(camera, RenderPassType_Forward);
			mRenderer.RenderSky();

			device.EndRenderBehavior();
			device.EndEvent();
		}

		// transparent
		RenderTransparents(mRBTransparent, RenderPassType_Forward);

		// postprocess
		RenderPostprocess(mRTMain);

		RenderPath2D::Render();
	}
}