#pragma once

#include "gui\guiInclude.h"
#include "gui\guiWidgets\widgets.h"
#include "gui\guiScheme.h"
#include "renderer\2D\sprite.h"
#include "renderer\2D\textDrawable.h"

namespace Cjing3D
{
	class GUIStage;

	class GUIRenderer
	{
	public:
		GUIRenderer(GUIStage& guiStage);
		~GUIRenderer();

		void Update(F32 dt);
		void Render();

		// gui scheme
		const Gui::GUIScheme& GetGUIScheme()const;

		// render
		void ApplyScissor(const Rect& rect);
		void ResetScissor();
		void RenderSprite(Sprite& sprite);
		void RenderText(TextDrawable* text);

	private:
		GUIStage& mGUIStage;
		Gui::GUIScheme mGUIScheme;
	};
}