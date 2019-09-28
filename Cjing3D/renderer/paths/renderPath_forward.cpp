#include "renderPath_forward.h"
#include "renderer\renderer.h"
#include "renderer\RHI\device.h"
#include "renderer\paths\renderImage.h"

namespace Cjing3D {

	RenderPathForward::RenderPathForward(Renderer& renderer) :
		RenderPath(renderer),
		mRTMain(nullptr),
		mRTFinal(nullptr)
	{
	}

	RenderPathForward::~RenderPathForward()
	{
	}

	void RenderPathForward::Initialize()
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

	void RenderPathForward::Render()
	{
		RenderScene();
		RenderComposition();
	}

	void RenderPathForward::Uninitialize()
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

	void RenderPathForward::Compose()
	{
		RenderImage::Render(mRTMain->GetTexture(), mRenderer);
	}

	void RenderPathForward::RenderComposition()
	{
		mRTFinal->Bind();
	}

	void RenderPathForward::SetupFixedState()
	{
		mRenderer.SetupRenderFrame();
	}

	void RenderPathForward::RenderScene()
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