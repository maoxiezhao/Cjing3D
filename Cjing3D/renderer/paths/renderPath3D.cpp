#include "renderPath3D.h"
#include "renderer\renderer.h"
#include "renderImage.h"
#include "renderer\textureHelper.h"

#include "renderer\2D\sprite.h"
#include "renderer\2D\renderer2D.h"

#include "system\component\genericType.h"

namespace Cjing3D
{
	RenderPath3D::RenderPath3D() :
		mRenderLayerMask(LayerComponent::DefaultLayerMask)
	{
	}

	RenderPath3D::~RenderPath3D()
	{
	}

	void RenderPath3D::Update(F32 dt)
	{
		RenderPath2D::Update(dt);

		Renderer::UpdatePerFrameData(dt, GetRenderLayerMask());
	}

	void RenderPath3D::ResizeBuffers()
	{
		RenderPath2D::ResizeBuffers();

		auto& device = Renderer::GetDevice();
		FORMAT backBufferFormat = device.GetBackBufferFormat();
		const auto screenSize = device.GetScreenSize();

		// depth buffer
		{
			TextureDesc desc = {};
			desc.mWidth = screenSize[0];
			desc.mHeight = screenSize[1];
			desc.mFormat = RenderPath3D::DepthStencilFormatAlias;
			desc.mBindFlags = BIND_DEPTH_STENCIL | BIND_SHADER_RESOURCE;

			const auto result = device.CreateTexture2D(&desc, nullptr, mDepthBuffer);
			Debug::ThrowIfFailed(result, "Failed to create depth buffer:%08x", result);
			device.SetResourceName(mDepthBuffer, "DepthBuffer");

			const auto tempResult = device.CreateTexture2D(&desc, nullptr, mDepthBufferTemp);
			Debug::ThrowIfFailed(tempResult, "Failed to create temp depth buffer:%08x", result);
			device.SetResourceName(mDepthBufferTemp, "DepthBufferTemp");
		}

		// linear depth buffer
		{
			TextureDesc desc = {};
			desc.mWidth = screenSize[0];
			desc.mHeight = screenSize[1];
			desc.mFormat = FORMAT_R32_FLOAT;
			desc.mMipLevels = 6;
			desc.mBindFlags = BIND_SHADER_RESOURCE | BIND_UNORDERED_ACCESS;

			const auto result = device.CreateTexture2D(&desc, nullptr, mDepthBufferLinear);
			Debug::ThrowIfFailed(result, "Failed to create linear depth buffer:%08x", result);
			device.SetResourceName(mDepthBufferLinear, "DepthBufferLinear");

			for (int i = 0; i < 6; i++)
			{
				device.CreateShaderResourceView(mDepthBufferLinear, 0, -1, i, 1);
				device.CreateUnordereddAccessView(mDepthBufferLinear, 0, -1, i);
			}
		}

		// AO Texture
		{
			TextureDesc desc = {};
			desc.mWidth = screenSize[0];
			desc.mHeight = screenSize[1];
			desc.mFormat = FORMAT_R8_UNORM;
			desc.mBindFlags = BIND_SHADER_RESOURCE | BIND_UNORDERED_ACCESS;

			const auto result = device.CreateTexture2D(&desc, nullptr, mAOTexture);
			Debug::ThrowIfFailed(result, "Failed to create ao texture:%08x", result);
			device.SetResourceName(mAOTexture, "AOTexture");		
		}

		// post process render target
		{
			TextureDesc desc = {};
			desc.mWidth = screenSize[0];
			desc.mHeight = screenSize[1];
			desc.mFormat = backBufferFormat;
			desc.mBindFlags = BIND_RENDER_TARGET | BIND_SHADER_RESOURCE | BIND_UNORDERED_ACCESS;

			HRESULT result = device.CreateTexture2D(&desc, nullptr, mRTPostprocessLDR1);
			Debug::ThrowIfFailed(result, "Failed to create postprocess render target:%08x", result);
			device.SetResourceName(mRTPostprocessLDR1, "RTPostprocessLDR1");

			result = device.CreateTexture2D(&desc, nullptr, mRTPostprocessLDR2);
			Debug::ThrowIfFailed(result, "Failed to create postprocess render target:%08x", result);
			device.SetResourceName(mRTPostprocessLDR2, "RTPostprocessLDR2");
		}
	}

	void RenderPath3D::Compose()
	{
		GraphicsDevice& device = Renderer::GetDevice();
		device.BeginEvent("Composition");

		Texture2D* lastTexture = GetLastPostprocessRT();
		if (lastTexture != nullptr && lastTexture->IsValid())
		{
			RenderImage::ImageParams params;
			params.EnableFullScreen();
			params.mBlendType = BlendType_Opaque;

			RenderImage::Render(*lastTexture, params);
		}

		RenderPath2D::Compose();

		device.EndEvent();
	}

	void RenderPath3D::RenderShadowmaps()
	{
		if (!IsShadowEnable()) {
			return;
		}

		Renderer::RenderShadowmaps(Renderer::GetCamera(), GetRenderLayerMask());
	}

	void RenderPath3D::RenderDepthLinear()
	{
		Renderer::RenderLinearDepth(mDepthBufferTemp, mDepthBufferLinear);
	}

	void RenderPath3D::RenderAO()
	{
		switch (mAOType)
		{
		case Cjing3D::RenderPath3D::AOTYPE_SSAO:
			Renderer::RenderSSAO(mDepthBufferTemp, mDepthBufferLinear, mAOTexture, GetAORange(), GetAOSampleCount());;
			break;
		default:
			break;
		}
	}

	void RenderPath3D::RenderTransparents(RenderBehavior& renderBehavior, RenderPassType renderType)
	{
		PROFILER_BEGIN_GPU_BLOCK("RenderTransparents");
		GraphicsDevice& device = Renderer::GetDevice();
		CameraComponent& camera = Renderer::GetCamera();

		device.BeginRenderBehavior(renderBehavior);
		const Texture* rtTexture = renderBehavior.mDesc.mParams[0].mTexture;

		ViewPort vp;
		vp.mWidth = (F32)rtTexture->GetDesc().mWidth;
		vp.mHeight = (F32)rtTexture->GetDesc().mHeight;
		vp.mMinDepth = 0.0f;
		vp.mMaxDepth = 1.0f;
		device.BindViewports(&vp, 1, GraphicsThread::GraphicsThread_IMMEDIATE);

		// transparent scene
		Renderer::RenderSceneTransparent(camera, renderType);

		// particles
		PROFILER_BEGIN_GPU_BLOCK("RenderParticles");
		Renderer::RenderParticles(camera, mDepthBufferLinear);
		PROFILER_END_BLOCK();

		// debug scene
		Renderer::RenderDebugScene(camera);

		device.EndRenderBehavior();

		PROFILER_END_BLOCK();
	}

	void RenderPath3D::RenderPostprocess(Texture2D & rtScreen)
	{
		Texture2D* rtRead = &rtScreen;
		Texture2D* rtWrite = GetLastPostprocessRT();

		// HDR-> LDR
		Renderer::PostprocessTonemap(*rtRead, *rtWrite, GetExposure());

		// FXAA
		if (IsFXAAEnable())
		{
			rtRead = rtWrite;
			rtWrite = &mRTPostprocessLDR1;

			Renderer::PostprocessFXAA(*rtRead, *rtWrite);
		}

		if (rtWrite != GetLastPostprocessRT()) {
			TextureHelper::SwapTexture(*rtWrite, *GetLastPostprocessRT());
		}
	}

	void RenderPath3D::RenderDebug()
	{
		
	}
}