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
		CommandList cmd = device.GetCommandList();
		Renderer::RefreshRenderData(cmd);
		
		// shadowmaps
		cmd = device.GetCommandList();
		RenderShadowmaps(cmd);

		// depth prepass
		{
			CommandList cmd = device.GetCommandList();
			// update main camera constant buffer
			Renderer::UpdateCameraCB(cmd, camera);

			PROFILER_BEGIN_GPU_BLOCK(cmd, "depthPrepass");
			device.BeginEvent(cmd, "depthPrepass");
			device.BeginRenderBehavior(cmd, mRBDepthPrepass);

			ViewPort vp;
			vp.mWidth = (F32)mDepthBuffer.GetDesc().mWidth;
			vp.mHeight = (F32)mDepthBuffer.GetDesc().mHeight;
			device.BindViewports(cmd, &vp, 1);

			Renderer::RenderSceneOpaque(cmd, camera, RenderPassType_Depth);

			device.EndRenderBehavior(cmd);
			device.EndEvent(cmd);
			PROFILER_END_BLOCK();
		}
		// opaque scene
		{
			PROFILER_BEGIN_GPU_BLOCK(cmd, "RenderSceneOpaque");
			device.BeginEvent(cmd, "RenderSceneOpaque");
			device.BeginRenderBehavior(cmd, mRBMain);

			ViewPort vp;
			vp.mWidth = (F32)mRTMain.GetDesc().mWidth;
			vp.mHeight = (F32)mRTMain.GetDesc().mHeight;
			device.BindViewports(cmd, &vp, 1);

			Renderer::RenderSceneOpaque(cmd, camera, RenderPassType_Forward);
			Renderer::RenderSky(cmd);

			device.EndRenderBehavior(cmd);
			device.EndEvent(cmd);
			PROFILER_END_BLOCK();
		}

		// transparent
		RenderTransparents(cmd, mRBTransparent, RenderPassType_Forward);

		// postprocess
		RenderPostprocess(cmd, mRTMain);

		RenderPath2D::Render();
	}
}