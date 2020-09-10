#pragma once

#include "gui\guiWidgets\widgets.h"

namespace Cjing3D {
	namespace Gui {

		class Image : public Widget
		{
		public:
			enum ImageStretchMode 
			{
				STRETCH_SCALE_ON_EXPAND,
				STRETCH_SCALE,
				STRETCH_TILE,	// TODO
				STRETCH_KEEP,
				STRETCH_KEEP_ASPECT
			};

			Image(GUIStage& stage, const StringID& name = StringID::EMPTY, const std::string& path = "");

			void SetExpand(bool isExpand) { mIsExpand = isExpand; }
			bool GetExpand()const { return mIsExpand; }
			void SetStretchMode(ImageStretchMode mode) { mStretchMode = mode; }
			ImageStretchMode GetStetchMode()const { return mStretchMode; }
			void SetColor(const Color4& color) { mImageColor = color; }
			Color4 GetColor()const { return mImageColor; }

			void SetTexture(const std::string& path);
			void SetTexture(const Sprite& sprite);
			void SetTexture(TextureResourcePtr texture);
			Sprite GetSprite();
			Texture2D* GetTexture();

			virtual F32x2 CalculateBestSize()const;

			Signal<void()> OnTextureChanged;

		protected:
			virtual void RenderImpl(const Rect& destRect);

			ImageStretchMode mStretchMode = STRETCH_SCALE_ON_EXPAND;
			bool mIsExpand = false;
			Sprite mSprite;
			Color4 mImageColor = Color4::White();
		};

	}
}