#pragma once

#include "gui\guiInclude.h"
#include "gui\guiWidgets\widgets.h"
#include "gui\guiScheme.h"
#include "renderer\2D\sprite.h"
#include "renderer\2D\textDrawable.h"

namespace Cjing3D
{
	class GUIStage;
	class IMGUIStage;

	class GUIRenderer
	{
	public:
		GUIRenderer(GUIStage& guiStage);
		~GUIRenderer();

		void Update(F32 dt);
		void Render();
		void SetImGuiStage(IMGUIStage* imGuiStage);

		// gui scheme
		const Gui::GUIScheme& GetGUIScheme()const;

		// render
		void RenderSprite(Sprite& sprite);
		void RenderText(TextDrawable* text);

	private:
		GUIStage& mGUIStage;
		IMGUIStage* mImGuiStage = nullptr;
		Gui::GUIScheme mGUIScheme;
	};
}