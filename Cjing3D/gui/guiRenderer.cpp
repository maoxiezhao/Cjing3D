#include "gui\guiRenderer.h"
#include "gui\guiStage.h"
#include "gui\imguiStage.h"
#include "renderer\2D\renderer2D.h"

namespace Cjing3D
{


	GUIRenderer::GUIRenderer(GUIStage& guiStage):
		mGUIStage(guiStage)
	{
		mGUIScheme.InitDefaultScheme();
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

	const Gui::GUIScheme& GUIRenderer::GetGUIScheme() const
	{
		return mGUIScheme;
	}

	void GUIRenderer::RenderSprite(Sprite& sprite)
	{
		Renderer2D::AddSprite(&sprite, false, "");
	}

	void GUIRenderer::RenderText(TextDrawable* text)
	{
		Renderer2D::AddTextDrawable(text, false, "");
	}
}