#include "forwardPass.h"
#include "renderer\renderer.h"
#include "renderer\RHI\device.h"

namespace Cjing3D {

	ForwardPass::ForwardPass(SystemContext& context, Renderer& renderer) :
		mContext(context),
		mRenderer(renderer),
		mIsInitialized(false),
		mRenderTarget(nullptr)
	{
		Initialize();
	}

	ForwardPass::~ForwardPass()
	{
		Uninitialize();
	}

	void ForwardPass::Initialize()
	{
		if (mIsInitialized == true) {
			return;
		}

		const auto screenSize = mRenderer.GetDevice().GetScreenSize();
		mRenderTarget = std::make_unique<RenderTarget>(mRenderer);
		mRenderTarget->Setup(screenSize[0], screenSize[1], FORMAT_R16G16B16A16_FLOAT, 1, true);

		mIsInitialized = true;
	}

	void ForwardPass::Render()
	{
		SetupFixedState();

		mRenderer.UpdateCameraCB(mRenderer.GetCamera());

		mRenderTarget->Bind(0, 0, 0, 0);
		{
			// draw
		}
		mRenderTarget->UnBind();
	}

	void ForwardPass::Uninitialize()
	{
		if (mIsInitialized == false) {
			return;
		}

		mRenderTarget->Clear();
		mRenderTarget = nullptr;

		mIsInitialized = false;
	}

	void ForwardPass::SetupFixedState()
	{
	}

}