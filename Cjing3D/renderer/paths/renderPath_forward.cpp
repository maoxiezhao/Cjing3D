#include "renderPath_forward.h"
#include "renderer\renderer.h"
#include "renderer\paths\renderImage.h"

namespace Cjing3D {

	RenderPathForward::RenderPathForward() 
	{
	}

	RenderPathForward::~RenderPathForward()
	{
	}

	void RenderPathForward::ResizeBuffers()
	{
		RenderPath3D::ResizeBuffers();
		
		auto& device = Renderer::GetDevice();
		const auto screenSize = device.GetScreenSize();
		{
			TextureDesc desc = {};
			desc.mWidth = screenSize[0];
			desc.mHeight = screenSize[1];
			desc.mFormat = RenderPath3D::RenderTargetFormatHDR;
			desc.mSampleDesc.mCount = 1;
			desc.mBindFlags = BIND_RENDER_TARGET | BIND_SHADER_RESOURCE | BIND_UNORDERED_ACCESS;
			{
				const auto result = device.CreateTexture2D(&desc, nullptr, mRTMain);
				Debug::ThrowIfFailed(result, "Failed to create render target:%08x", result);

				device.SetResourceName(mRTMain, "RTMain");
			}
		}
		//////////////////////////////////////////////////////////////////////////////////////////
		{
			RenderBehaviorDesc desc = {};
			desc.mParams.push_back({ RenderBehaviorParam::RenderType_DepthStencil, GetDepthBuffer(), -1, RenderBehaviorParam::RenderOperation_Clear });

			device.CreateRenderBehavior(desc, mRBDepthPrepass);
		}
		{
			RenderBehaviorDesc desc = {};
			desc.mParams.push_back({ RenderBehaviorParam::RenderType_RenderTarget, &mRTMain,         -1, RenderBehaviorParam::RenderOperation_Clear });
			desc.mParams.push_back({ RenderBehaviorParam::RenderType_DepthStencil, GetDepthBuffer(), -1, RenderBehaviorParam::RenderOperation_Load });

			device.CreateRenderBehavior(desc, mRBMain);
		}
		{
			RenderBehaviorDesc desc = {};
			desc.mParams.push_back({ RenderBehaviorParam::RenderType_RenderTarget, &mRTMain,         -1, RenderBehaviorParam::RenderOperation_Load });
			desc.mParams.push_back({ RenderBehaviorParam::RenderType_DepthStencil, GetDepthBuffer(), -1, RenderBehaviorParam::RenderOperation_Load });

			device.CreateRenderBehavior(desc, mRBTransparent);
		}
	}

	void RenderPathForward::Render()
	{
		GraphicsDevice& device = Renderer::GetDevice();
		CameraComponent& camera = Renderer::GetCamera();

		// update and bind common resources
		Renderer::RefreshRenderData();

		// bind constant buffer
		Renderer::BindCommonResource();
		Renderer::BindConstanceBuffer(SHADERSTAGES_VS);
		Renderer::BindConstanceBuffer(SHADERSTAGES_PS);
		
		// shadowmaps
		RenderShadowmaps();

		// update main camera constant buffer
		Renderer::UpdateCameraCB(camera);

		// depth prepass
		{
			PROFILER_BEGIN_GPU_BLOCK("depthPrepass");
			device.BeginEvent("depthPrepass");
			device.BeginRenderBehavior(mRBDepthPrepass);

			ViewPort vp;
			vp.mWidth = (F32)mDepthBuffer.GetDesc().mWidth;
			vp.mHeight = (F32)mDepthBuffer.GetDesc().mHeight;
			device.BindViewports(&vp, 1, GraphicsThread::GraphicsThread_IMMEDIATE);

			Renderer::RenderSceneOpaque(camera, RenderPassType_Depth);

			device.EndRenderBehavior();
			device.EndEvent();
			PROFILER_END_BLOCK();
		}
		// opaque scene
		{
			PROFILER_BEGIN_GPU_BLOCK("RenderSceneOpaque");
			device.BeginEvent("RenderSceneOpaque");
			device.BeginRenderBehavior(mRBMain);

			ViewPort vp;
			vp.mWidth = (F32)mRTMain.GetDesc().mWidth;
			vp.mHeight = (F32)mRTMain.GetDesc().mHeight;
			vp.mMinDepth = 0.0f;
			vp.mMaxDepth = 1.0f;
			device.BindViewports(&vp, 1, GraphicsThread::GraphicsThread_IMMEDIATE);

			Renderer::RenderSceneOpaque(camera, RenderPassType_Forward);
			Renderer::RenderSky();

			device.EndRenderBehavior();
			device.EndEvent();
			PROFILER_END_BLOCK();
		}

		// transparent
		RenderTransparents(mRBTransparent, RenderPassType_Forward);

		// postprocess
		RenderPostprocess(mRTMain);

		RenderPath2D::Render();
	}
}