#pragma once

#include "gui\guiWidgets\widgets.h"

namespace Cjing3D {
	namespace Gui {

		class NinePatchImage : public Widget
		{
		public:
			NinePatchImage(GUIStage& stage, const StringID& name = StringID::EMPTY, const std::string& path = "");

			void SetTexture(const std::string& path);
			void SetTexture(const Sprite& sprite);
			void SetTexture(TextureResourcePtr texture);
			Sprite GetSprite();
			Texture2D* GetTexture();

			void SetPatchRect(const Rect& rect);
			void SetPatchRect(Rect::RectMargin margin, F32 value);

			Signal<void()> OnTextureChanged;

			virtual F32x2 CalculateBestSize()const;
		protected:
			virtual void RenderImpl(const Rect& destRect);

		private:
			Rect mPatchRect;
			Sprite mImage;
		};

	}
}