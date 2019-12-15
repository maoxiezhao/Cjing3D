#include "guiRenderer.h"
#include "imguiStage.h"

namespace Cjing3D
{
	GUIRenderer::GUIRenderer()
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
		if (mImGuiStage != nullptr) {
			mImGuiStage->RenderImpl();
		}
	}

	void GUIRenderer::SetImGuiStage(IMGUIStage* imGuiStage)
	{
		mImGuiStage = imGuiStage;
	}
}