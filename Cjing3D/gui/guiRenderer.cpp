#include "gui\guiRenderer.h"
#include "gui\guiStage.h"
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
	}

	const Gui::GUIScheme& GUIRenderer::GetGUIScheme() const
	{
		return mGUIScheme;
	}

	void GUIRenderer::ApplyScissor(const Rect& rect)
	{
		Rect scissorRect = rect;
		if (scissorRect.mLeft > scissorRect.mRight) {
			scissorRect.mLeft = scissorRect.mRight;
		}
		if (scissorRect.mTop > scissorRect.mBottom) {
			scissorRect.mTop = scissorRect.mBottom;
		}

		RectInt recti;
		recti.mBottom = (I32)scissorRect.mBottom;
		recti.mLeft   = (I32)scissorRect.mLeft;
		recti.mRight  = (I32)scissorRect.mRight;
		recti.mTop    = (I32)scissorRect.mTop;
		Renderer2D::AddScissorRect(recti);
	}

	void GUIRenderer::ResetScissor()
	{
		RectInt rect;
		rect.mBottom = INT32_MAX;
		rect.mLeft = INT32_MIN;
		rect.mRight = INT32_MAX;
		rect.mTop = INT32_MIN;
		Renderer2D::AddScissorRect(rect);
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