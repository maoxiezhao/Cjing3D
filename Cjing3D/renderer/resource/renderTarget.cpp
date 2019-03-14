#include "renderTarget.h"
#include "renderer\renderer.h"
#include "renderer\RHI\device.h"

namespace Cjing3D
{
	DepthTarget::DepthTarget(Renderer & renderer):
		mRenderer(renderer),
		mTexture(nullptr)
	{
	}

	DepthTarget::~DepthTarget()
	{
	}

	void DepthTarget::Setup(U32 width, U32 height)
	{
		TextureDesc desc = {};
		desc.mWidth = width;
		desc.mHeight = height;
		desc.mMipLevels = 1;
		desc.mFormat = FORMAT_R32G8X24_TYPELESS;	// ????
		desc.mBindFlags = BIND_DEPTH_STENCIL | BIND_SHADER_RESOURCE;

		mTexture = std::make_shared<RhiTexture2D>(mRenderer.GetDevice());
		mTexture->SetDesc(desc);

		const auto result = mRenderer.GetDevice().CreateTexture2D(&desc, nullptr, *mTexture);
		Debug::ThrowIfFailed(result, "Failed to create depth target:%08x", result);
	}

	void DepthTarget::Clear()
	{
		if (mTexture != nullptr)
		{
			mRenderer.GetDevice().ClearDepthStencil(*mTexture, CLEAR_DEPTH | CLEAR_STENCIL, 0.0f, 0);
		}
	}

	RhiTexture2DPtr DepthTarget::GetTexture()
	{
		return mTexture;
	}

	RenderTarget::RenderTarget(Renderer& renderer):
		mRenderer(renderer)
	{
	}

	RenderTarget::~RenderTarget()
	{
	}

	void RenderTarget::Setup(U32 width, U32 height, FORMAT format, U32 mipMapLevelCount, bool hasDepth)
	{
		Clear();

		TextureDesc desc = {};
		desc.mWidth = width;
		desc.mHeight = height;
		desc.mMipLevels = mipMapLevelCount;
		desc.mFormat = format;
		desc.mBindFlags = BIND_RENDER_TARGET | BIND_SHADER_RESOURCE;
		
		RhiTexture2DPtr newTexture = std::make_shared<RhiTexture2D>(mRenderer.GetDevice());
		newTexture->SetDesc(desc);

		// Éú³Émipmap
		if (mipMapLevelCount > 1)
		{

		}

		const auto result = mRenderer.GetDevice().CreateTexture2D(&desc, nullptr, *newTexture);
		Debug::ThrowIfFailed(result, "Failed to create render target:%08x", result);

		mRenderTargets.push_back(newTexture);

		mViewPort.mWidth = width;
		mViewPort.mHeight = height;
		mViewPort.mMinDepth = 0.0f;
		mViewPort.mMaxDepth = 1.0f;

		if (hasDepth == true)
		{
			mDepthTarget = std::make_shared<DepthTarget>(mRenderer);
			mDepthTarget->Setup(width, height);
		}
	}

	void RenderTarget::Clear()
	{
		mRenderTargets.clear();

		if (mDepthTarget != nullptr)
		{
			mDepthTarget->Clear();
			mDepthTarget = nullptr;
		}
	}

	void RenderTarget::Bind()
	{
		auto& device = mRenderer.GetDevice();
		device.BindViewports(&mViewPort, 1, GraphicsThread_IMMEDIATE);

		// bind rendertarget
		std::vector<RhiTexture2D*> renderTargets;
		for (auto& texture : mRenderTargets) {
			renderTargets.push_back(texture.get());
		}
		auto depthTexture = (mDepthTarget != nullptr) ? mDepthTarget->GetTexture() : nullptr;
		device.BindRenderTarget(mNumViews, (RhiTexture2D**)renderTargets.data(), depthTexture != nullptr ? depthTexture.get() : nullptr);

	}

	void RenderTarget::BindAndClear(F32 r, F32 g, F32 b, F32 a)
	{
		auto& device = mRenderer.GetDevice();
		Bind();

		// clear renderTarget
		std::vector<RhiTexture2D*> renderTargets;
		for (auto& texture : mRenderTargets) {
			renderTargets.push_back(texture.get());
		}

		const F32x4 clearColor = { r, g, b, a };
		for (auto& texture : renderTargets)
		{
			device.ClearRenderTarget(*texture, clearColor);
		}

		// clear depth
		if (mDepthTarget != nullptr) {
			mDepthTarget->Clear();
		}
	}

	void RenderTarget::UnBind()
	{
		mRenderer.GetDevice().BindRenderTarget(0, nullptr, nullptr);
	}
}