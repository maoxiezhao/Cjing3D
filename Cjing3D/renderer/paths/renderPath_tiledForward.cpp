#include "renderPath_tiledForward.h"
#include "renderer\renderer.h"
#include "renderer\paths\renderImage.h"
#include "renderer\textureHelper.h"

namespace Cjing3D
{
	RenderPathTiledForward::RenderPathTiledForward()
	{
	}

	RenderPathTiledForward::~RenderPathTiledForward()
	{
	}

	void RenderPathTiledForward::Render()
	{
		GraphicsDevice& device = Renderer::GetDevice();
		CameraComponent& camera = Renderer::GetCamera();

	
		// update and bind common resources
		CommandList cmd = device.GetCommandList();
		Renderer::RefreshRenderData(cmd);

		// shadowmaps
		cmd = device.GetCommandList();
		RenderPath3D::RenderShadowmaps(cmd);

		// depth prepass
		{
			CommandList cmd = device.GetCommandList();
			Renderer::UpdateCameraCB(cmd, camera);

			PROFILER_BEGIN_GPU_BLOCK(cmd, "depthPrepass");
			device.BeginEvent(cmd, "depthPrepass");
			device.BeginRenderBehavior(cmd, mRBDepthPrepass);

			ViewPort vp;
			vp.mWidth = (F32)mDepthBuffer.GetDesc().mWidth;
			vp.mHeight = (F32)mDepthBuffer.GetDesc().mHeight;
			device.BindViewports(cmd , &vp, 1);

			Renderer::RenderSceneOpaque(cmd, camera, RenderPassType_Depth);

			device.EndRenderBehavior(cmd);
			device.EndEvent(cmd);
			device.CopyGPUResource(cmd, *GetDepthBufferTemp(), *GetDepthBuffer());
			PROFILER_END_BLOCK();

			RenderPath3D::RenderDepthLinear(cmd);
			RenderPath3D::RenderAO(cmd);
		}

		// opaque scene
		{
			CommandList cmd = device.GetCommandList();
			Renderer::TiledLightCulling(cmd , *GetDepthBufferTemp());

			PROFILER_BEGIN_GPU_BLOCK(cmd, "RenderSceneOpaque");
			device.BeginEvent(cmd, "RenderSceneOpaque");
			device.BeginRenderBehavior(cmd, mRBMain);

			ViewPort vp;
			vp.mWidth = (F32)mRTMain.GetDesc().mWidth;
			vp.mHeight = (F32)mRTMain.GetDesc().mHeight;
			device.BindViewports(cmd , &vp, 1);
			device.BindGPUResource(cmd, SHADERSTAGES_PS, IsAOEnable() ? GetAOTexture() : TextureHelper::GetWhite(), TEXTURE_SLOT_AO);

			Renderer::RenderSceneOpaque(cmd, camera, RenderPassType_TiledForward);
			Renderer::RenderSky(cmd);

			device.EndRenderBehavior(cmd);
			device.EndEvent(cmd);
			PROFILER_END_BLOCK();
		}

		// transparent/post process/ debug scene
		{
			CommandList cmd = device.GetCommandList();
			Renderer::UpdateCameraCB(cmd, camera);
			Renderer::BindCommonResource(cmd);

			// transparent
			RenderPath3D::RenderTransparents(cmd, mRBTransparent, RenderPassType_TiledForward);

			// postprocess
			RenderPath3D::RenderPostprocess(cmd, mRTMain);
		}

		// render 2D
		RenderPath2D::Render();
	}
}