#pragma once

#include "gui\guiInclude.h"
#include "gui\guiWidgets\widgets.h"
#include "renderer\2D\sprite.h"
#include "renderer\2D\textDrawable.h"

namespace Cjing3D
{
	class GUIStage;
	class IMGUIStage;

	class GUIScheme
	{
	public:
		// panel
		static StringID PanelBackground;
		// button	
		static StringID ButtonBackgroundBase;
		static StringID ButtonBackgroundClick;
		static StringID ButtonBackgroundHovered;
		static StringID ButtonTextBase;
		static StringID ButtonTextClick;
		static StringID ButtonTextHovered;

		void RegisterColor(const StringID& name, const Color4& color);
		void RegisterImg(const StringID& name);

		Color4 GetColor(const StringID& name)const;

	private:
		std::map<StringID, Color4> mRegisteredColor;
	};

	class GUIRenderer
	{
	public:
		GUIRenderer(GUIStage& guiStage);
		~GUIRenderer();

		void Update(F32 dt);
		void Render();
		void SetImGuiStage(IMGUIStage* imGuiStage);

		// gui scheme
		void InitDefaultScheme();
		const GUIScheme& GetGUIScheme()const;

		// render
		void RenderSprite(Sprite& sprite);
		void RenderText(TextDrawable* text);

	private:
		GUIStage& mGUIStage;
		IMGUIStage* mImGuiStage = nullptr;
		GUIScheme mGUIScheme;
	};
}