#include "gui\guiRenderer.h"
#include "gui\guiStage.h"
#include "gui\imguiStage.h"
#include "renderer\2D\renderer2D.h"

namespace Cjing3D
{
#define REGISTER_SCHEME_NAME(name) StringID name = StringID(#name)
	// panel
	REGISTER_SCHEME_NAME(GUIScheme::PanelBackground);
	// button
	REGISTER_SCHEME_NAME(GUIScheme::ButtonBackgroundBase);
	REGISTER_SCHEME_NAME(GUIScheme::ButtonBackgroundClick);
	REGISTER_SCHEME_NAME(GUIScheme::ButtonBackgroundHovered);
	REGISTER_SCHEME_NAME(GUIScheme::ButtonTextBase);
	REGISTER_SCHEME_NAME(GUIScheme::ButtonTextClick);
	REGISTER_SCHEME_NAME(GUIScheme::ButtonTextHovered);
	// list item
	REGISTER_SCHEME_NAME(GUIScheme::ListItemBackgroundHovered);
	REGISTER_SCHEME_NAME(GUIScheme::ListItemBackgroundPressed);
	REGISTER_SCHEME_NAME(GUIScheme::ListItemBackgroundSelected);

#undef  REGISTER_SCHEME_NAME

	void GUIScheme::RegisterColor(const StringID& name, const Color4& color)
	{
		mRegisteredColor[name] = color;
	}

	void GUIScheme::RegisterImg(const StringID& name)
	{
		// TODO
	}

	Color4 GUIScheme::GetColor(const StringID& name) const
	{
		auto it = mRegisteredColor.find(name);
		if (it != mRegisteredColor.end())
		{
			return it->second;
		}
		return Color4::White();
	}

	GUIRenderer::GUIRenderer(GUIStage& guiStage):
		mGUIStage(guiStage)
	{
		InitDefaultScheme();
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

	void GUIRenderer::InitDefaultScheme()
	{
		// panel
		mGUIScheme.RegisterColor(GUIScheme::PanelBackground, Color4(45, 45, 48, 225));
		// button
		mGUIScheme.RegisterColor(GUIScheme::ButtonBackgroundBase, Color4(0, 132, 190, 255));
		mGUIScheme.RegisterColor(GUIScheme::ButtonBackgroundClick, Color4(0, 107, 162, 255));
		mGUIScheme.RegisterColor(GUIScheme::ButtonBackgroundHovered, Color4(0, 123, 182, 255));
		mGUIScheme.RegisterColor(GUIScheme::ButtonTextBase, Color4(251, 250, 248, 255));
		mGUIScheme.RegisterColor(GUIScheme::ButtonTextClick, Color4(251, 250, 248, 255));
		mGUIScheme.RegisterColor(GUIScheme::ButtonTextHovered, Color4(251, 250, 248, 255));
		// list item
		mGUIScheme.RegisterColor(GUIScheme::ListItemBackgroundHovered, Color4(68, 67, 65, 255));
		mGUIScheme.RegisterColor(GUIScheme::ListItemBackgroundPressed, Color4(0, 123, 182, 255));
		mGUIScheme.RegisterColor(GUIScheme::ListItemBackgroundSelected, Color4(110, 108, 109, 255));
	}

	const GUIScheme& GUIRenderer::GetGUIScheme() const
	{
		return mGUIScheme;
	}

	void GUIRenderer::RenderSprite(Sprite& sprite)
	{
		Renderer2D::RequestRenderSprite(&sprite);
	}

	void GUIRenderer::RenderText(TextDrawable* text)
	{
		Renderer2D::RequestRenderextDrawable(text);
	}
}