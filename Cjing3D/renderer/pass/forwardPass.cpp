#include "forwardPass.h"
#include "renderer\renderer.h"
#include "renderer\RHI\device.h"
#include "renderer\renderImage.h"

namespace Cjing3D {

	ForwardPass::ForwardPass(SystemContext& context, Renderer& renderer) :
		mContext(context),
		mRenderer(renderer),
		mIsInitialized(false),
		mRTMain(nullptr),
		mRTFinal(nullptr)
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
		mRTMain = std::make_unique<RenderTarget>(mRenderer);
		mRTMain->Setup(screenSize[0], screenSize[1], FORMAT_R16G16B16A16_FLOAT, 1, true);

		mRTFinal = std::make_unique<RenderTarget>(mRenderer);
		mRTFinal->Setup(screenSize[0], screenSize[1], FORMAT_R16G16B16A16_FLOAT, 1, false);

		mIsInitialized = true;
	}

	void ForwardPass::Render()
	{
		RenderScene();
		RenderComposition();
	}

	void ForwardPass::Uninitialize()
	{
		if (mIsInitialized == false) {
			return;
		}

		mRTMain->Clear();
		mRTMain = nullptr;

		mRTFinal->Clear();
		mRTFinal = nullptr;

		mIsInitialized = false;
	}

	void ForwardPass::Compose()
	{
		RenderImage::Render(mRTFinal->GetTexture(), mRenderer);
	}

	void ForwardPass::RenderComposition()
	{
		mRTFinal->Bind();
	}

	void ForwardPass::SetupFixedState()
	{
	}

	void ForwardPass::RenderScene()
	{
		auto camera = mRenderer.GetCamera();
		if (camera == nullptr) {
			return;
		}

		SetupFixedState();

		mRenderer.UpdateCameraCB(*camera);

		mRTMain->BindAndClear(0, 0, 0, 0);
		{
			mRenderer.RenderSceneOpaque(camera, ShaderType_Forward);
		}
		mRTMain->UnBind();
	}

}