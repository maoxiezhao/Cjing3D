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

		if (!mResolutionChangedConn.IsConnected())
		{
			ResizeBuffers();
			mResolutionChangedConn = EventSystem::Register(EVENT_RESOLUTION_CHANGE,
				[this](const VariantArray& variants) {
					ResizeBuffers();
				});
		}
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
		CommandList cmd = device.GetCommandList();
		device.BeginEvent(cmd, "Render2D");
		{
			device.BeginRenderBehavior(cmd, mRBFinal);

			ViewPort vp;
			vp.mWidth = (F32)mRTFinal.GetDesc().mWidth;
			vp.mHeight = (F32)mRTFinal.GetDesc().mHeight;
			vp.mMinDepth = 0.0f;
			vp.mMaxDepth = 1.0f;
			device.BindViewports(cmd , &vp, 1);
			
			RectInt rect;
			rect.mBottom = INT32_MAX;
			rect.mLeft = INT32_MIN;
			rect.mRight = INT32_MAX;
			rect.mTop = INT32_MIN;
			device.BindScissorRects(cmd, 1, &rect);

			RenderGUI(cmd);
			Render2D(cmd);

			device.EndRenderBehavior(cmd);
		}
		device.EndEvent(cmd);

		RenderPath::Render();
	}

	void RenderPath2D::RenderGUI(CommandList cmd)
	{
		GraphicsDevice& device = Renderer::GetDevice();
		device.BeginEvent(cmd, "RenderGUI");

		// 从guiStage中获取guiRenderer执行渲染过程
		GetGlobalContext().gGUIStage->GetGUIRenderer().Render();
	
		device.EndEvent(cmd);
	}

	void RenderPath2D::Render2D(CommandList cmd)
	{
		Renderer::GetDevice().BeginEvent(cmd,"Render2D");
		Renderer2D::Render2D(cmd);
		Renderer::GetDevice().EndEvent(cmd);
	}

	void RenderPath2D::Compose(CommandList cmd)
	{
		if (mRTFinal.IsValid())
		{
			RenderImage::ImageParams params;
			params.EnableFullScreen();
			params.mBlendType = BlendType_PreMultiplied;

			RenderImage::Render(cmd, mRTFinal, params);
		}

		RenderPath::Compose(cmd);
	}
}