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
		Renderer::RefreshRenderData();

		// bind constant buffer
		Renderer::BindCommonResource();
		Renderer::BindConstanceBuffer(SHADERSTAGES_VS);
		Renderer::BindConstanceBuffer(SHADERSTAGES_PS);

		//reset scissor rect
		RectInt rect;
		rect.mBottom = INT32_MAX;
		rect.mLeft = INT32_MIN;
		rect.mRight = INT32_MAX;
		rect.mTop = INT32_MIN;
		device.BindScissorRects(1, &rect);

		// shadowmaps
		RenderPath3D::RenderShadowmaps();

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
			device.CopyGPUResource(*GetDepthBufferTemp(), *GetDepthBuffer());
			PROFILER_END_BLOCK();

			RenderPath3D::RenderDepthLinear();
			RenderPath3D::RenderAO();
		}

		// opaque scene
		{
			Renderer::TiledLightCulling(*GetDepthBufferTemp());

			PROFILER_BEGIN_GPU_BLOCK("RenderSceneOpaque");
			device.BeginEvent("RenderSceneOpaque");
			device.BeginRenderBehavior(mRBMain);

			ViewPort vp;
			vp.mWidth = (F32)mRTMain.GetDesc().mWidth;
			vp.mHeight = (F32)mRTMain.GetDesc().mHeight;
			vp.mMinDepth = 0.0f;
			vp.mMaxDepth = 1.0f;
			device.BindViewports(&vp, 1, GraphicsThread::GraphicsThread_IMMEDIATE);

			device.BindGPUResource(SHADERSTAGES_PS, IsAOEnable() ? GetAOTexture() : TextureHelper::GetWhite(), TEXTURE_SLOT_AO);

			Renderer::RenderSceneOpaque(camera, RenderPassType_TiledForward);
			Renderer::RenderSky();

			device.EndRenderBehavior();
			device.EndEvent();
			PROFILER_END_BLOCK();
		}

		// transparent
		RenderPath3D::RenderTransparents(mRBTransparent, RenderPassType_TiledForward);

		// postprocess
		RenderPath3D::RenderPostprocess(mRTMain);

		// render debug
		RenderPath3D::RenderDebug();

		RenderPath2D::Render();
	}
}