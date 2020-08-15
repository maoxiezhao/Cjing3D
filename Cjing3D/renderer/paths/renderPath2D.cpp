#include "renderPath2D.h"
#include "renderer\renderer.h"
#include "renderer\paths\renderImage.h"
#include "core\globalContext.hpp"
#include "gui\guiStage.h"
#include "renderer\2D\renderer2D.h"

namespace Cjing3D
{
	RenderPath2D::RenderPath2D()
	{
	}

	RenderPath2D::~RenderPath2D()
	{
	}

	void RenderPath2D::ResizeBuffers()
	{
		RenderPath::ResizeBuffers();

		auto& device = Renderer::GetDevice();
		const auto screenSize = device.GetScreenSize();
		FORMAT format = device.GetBackBufferFormat();

		TextureDesc desc = {};
		desc.mWidth = screenSize[0];
		desc.mHeight = screenSize[1];
		desc.mFormat = format;
		desc.mBindFlags = BIND_RENDER_TARGET | BIND_SHADER_RESOURCE;
		desc.mClearValue.color[3] = 0.0f;
		{
			const auto result = device.CreateTexture2D(&desc, nullptr, mRTFinal);
			Debug::ThrowIfFailed(result, "Failed to create render target:%08x", result);
			device.SetResourceName(mRTFinal, "mRTFinal");
		}

		//////////////////////////////////////////////////////////////////////////////////////////
		{
			RenderBehaviorDesc desc = {};
			desc.mParams.push_back({ RenderBehaviorParam::RenderType_RenderTarget, &mRTFinal,         -1, RenderBehaviorParam::RenderOperation_Clear });
			desc.mParams.push_back({ RenderBehaviorParam::RenderType_DepthStencil, GetDepthBuffer(),  -1, RenderBehaviorParam::RenderOperation_Load });

			device.CreateRenderBehavior(desc, mRBFinal);
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
		GraphicsDevice& device = Renderer::GetDevice();
		device.BeginEvent("Render2D");
		{
			device.BeginRenderBehavior(mRBFinal);

			ViewPort vp;
			vp.mWidth = (F32)mRTFinal.GetDesc().mWidth;
			vp.mHeight = (F32)mRTFinal.GetDesc().mHeight;
			vp.mMinDepth = 0.0f;
			vp.mMaxDepth = 1.0f;
			device.BindViewports(&vp, 1, GraphicsThread::GraphicsThread_IMMEDIATE);
			
			RectInt rect;
			rect.mBottom = INT32_MAX;
			rect.mLeft = INT32_MIN;
			rect.mRight = INT32_MAX;
			rect.mTop = INT32_MIN;
			device.BindScissorRects(1, &rect);

			RenderGUI();
			Render2D();

			device.EndRenderBehavior();
		}

		device.EndEvent();

		RenderPath::Render();
	}

	void RenderPath2D::RenderGUI()
	{
		GraphicsDevice& device = Renderer::GetDevice();
		device.BeginEvent("RenderGUI");

		// 从guiStage中获取guiRenderer执行渲染过程
		GUIStage& guiStage = GlobalGetSubSystem<GUIStage>();
		guiStage.GetGUIRenderer().Render();
	
		device.EndEvent();
	}

	void RenderPath2D::Render2D()
	{
		Renderer::GetDevice().BeginEvent("2D");
		Renderer2D::Render2D();
		Renderer::GetDevice().EndEvent();
	}

	void RenderPath2D::Compose()
	{
		if (mRTFinal.IsValid())
		{
			RenderImage::ImageParams params;
			params.EnableFullScreen();
			params.mBlendType = BlendType_PreMultiplied;

			RenderImage::Render(mRTFinal, params);
		}

		RenderPath::Compose();
	}
}