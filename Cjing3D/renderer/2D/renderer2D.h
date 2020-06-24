#pragma once

#include "renderer\renderableCommon.h"
#include "renderer\paths\renderPath2D.h"
#include "renderer\rendererUtils.h"
#include "renderer\2D\sprite.h"
#include "renderer\2D\textDrawable.h"

namespace Cjing3D
{
	class SpritieRenderBatchQueue;

	namespace Renderer2D
	{
		void Initialize();
		void Uninitialize();
		void RenderSprites();
		void RenderTextDrawables();

		// sprite
		void AddSprite(Sprite* sprite);
		void RemoveSprite(Sprite* sprite);
		void RequestRenderSprite(Sprite* sprite);

		// font
		void AddTextDrawable(TextDrawable* text);
		void RemoveTextDrawable(TextDrawable* text);
		void RequestRenderextDrawable(TextDrawable* text);
	};

}