#include "guiRenderer.h"
#include "guiStage.h"
#include "imguiStage.h"

namespace Cjing3D
{
	GUIRenderer::GUIRenderer(GUIStage& guiStage):
		mGUIStage(guiStage)
	{
	}

	GUIRenderer::~GUIRenderer()
	{
	}

	void GUIRenderer::Update(F32 dt)
	{
	}

	void GUIRenderer::Render()
	{
		mGUIStage.RenderImpl();

		if (mImGuiStage != nullptr) {
			mImGuiStage->RenderImpl();
		}
	}

	void GUIRenderer::SetImGuiStage(IMGUIStage* imGuiStage)
	{
		mImGuiStage = imGuiStage;
	}
}