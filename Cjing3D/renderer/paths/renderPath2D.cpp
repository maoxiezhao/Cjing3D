#include "renderPath2D.h"
#include "renderer\renderer.h"
#include "renderer\paths\renderImage.h"
#include "core\systemContext.hpp"
#include "gui\guiStage.h"

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
		desc.mClearValue.color[3] = 0.0f;
		{
			const auto result = mRenderer.GetDevice().CreateTexture2D(&desc, nullptr, mRTFinal);
			Debug::ThrowIfFailed(result, "Failed to create render target:%08x", result);

			mRenderer.GetDevice().SetResourceName(mRTFinal, "mRTFinal");
		}

		//////////////////////////////////////////////////////////////////////////////////////////
		{
			RenderBehaviorDesc desc = {};
			desc.mParams.push_back({ RenderBehaviorParam::RenderType_RenderTarget, &mRTFinal,         -1, RenderBehaviorParam::RenderOperation_Clear });
			desc.mParams.push_back({ RenderBehaviorParam::RenderType_DepthStencil, GetDepthBuffer(),  -1, RenderBehaviorParam::RenderOperation_Load });

			mRenderer.GetDevice().CreateRenderBehavior(desc, mRBFinal);
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
		{
			device.BeginRenderBehavior(mRBFinal);

			ViewPort vp;
			vp.mWidth = (F32)mRTFinal.GetDesc().mWidth;
			vp.mHeight = (F32)mRTFinal.GetDesc().mHeight;
			vp.mMinDepth = 0.0f;
			vp.mMaxDepth = 1.0f;
			device.BindViewports(&vp, 1, GraphicsThread::GraphicsThread_IMMEDIATE);

			RenderGUI();

			device.EndRenderBehavior();
		}

		device.EndEvent();

		RenderPath::Render();
	}

	void RenderPath2D::RenderGUI()
	{
		GraphicsDevice& device = mRenderer.GetDevice();
		device.BeginEvent("RenderGUI");

		// 从guiStage中获取guiRenderer执行渲染过程
		GUIStage& guiStage = SystemContext::GetSystemContext().GetSubSystem<GUIStage>();
		guiStage.GetGUIRenderer().Render();
	
		device.EndEvent();
	}

	void RenderPath2D::Compose()
	{
		if (mRTFinal.IsValid())
		{
			RenderImage::ImageParams params;
			params.EnableFullScreen();
			params.mBlendType = BlendType_PreMultiplied;

			RenderImage::Render(mRTFinal, params, mRenderer);
		}

		RenderPath::Compose();
	}
}