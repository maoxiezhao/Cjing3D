#pragma once

#include "renderer\renderableCommon.h"
#include "renderer\paths\renderPath2D.h"
#include "renderer\rendererUtils.h"
#include "renderer\2D\sprite.h"
#include "renderer\2D\textDrawable.h"

namespace Cjing3D
{
	class SpritieRenderBatchQueue;

	struct RenderItem2D
	{
		enum RenderItemType
		{
			RenderItemType_None,
			RenderItemType_Sprite,
			RenderItemType_Font,
			RenderItemType_Scissor,
		};
		RenderItemType mType = RenderItemType_None;
		Sprite* mSprite = nullptr;
		TextDrawable* mText = nullptr;
		RectInt mScissorRect;
		bool mIsPersistent = false;
	};

	struct RenderLayer2D 
	{
		std::vector<RenderItem2D> mItems;
		StringID mLayerName;
		U32 mSort = 0;
	};

	namespace Renderer2D
	{
		void Initialize();
		void Uninitialize();
		void Render2D();

		// sprite
		void AddSprite(Sprite* sprite, bool isPersistent = true, const StringID& layerName = "");
		void RemoveSprite(Sprite* sprite);
		void RemoveSprite(Sprite* sprite, const StringID& layerName = "");

		// font
		void AddTextDrawable(TextDrawable* text, bool isPersistent = true, const StringID& layerName = "");
		void RemoveTextDrawable(TextDrawable* text);
		void RemoveTextDrawable(TextDrawable* text, const StringID& layerName = "");

		// scissor
		void AddScissorRect(const RectInt& rect, const StringID& layerName = "");

		// render layer
		void AddLayer(const StringID& name);
		void RemoveLayer(const StringID& name);
		void SortLayers();
		void CleanLayers();
	};

}