#pragma once

#include "gui\guiWidgets\widgets.h"

namespace Cjing3D {
	namespace Gui {

		class Button : public Widget
		{
		public:
			enum ButtonState
			{
				ButtonState_Base = 1 << 1,
				ButtonState_Hovered = 1 << 2,
				ButtonState_Pressed = 1 << 3
			};

			Button(GUIStage& stage, const StringID& name = StringID::EMPTY);

			void SetText(const UTF8String& text);
			void SetTextAlignment(Font::TextAlignH align);
			UTF8String GetText()const;
			TextDrawable& GetButtonLable();

			Signal<void()> OnClickCallback;

		private:
			virtual void RenderImpl(const Rect& destRect);

			void OnMouseEnter(const VariantArray& variants);
			void OnMouseLeave(const VariantArray& variants);
			void OnMousePressed(const VariantArray& variants);
			void OnMouseReleased(const VariantArray& variants);
			void OnMouseClick(const VariantArray& variants);

			U32 mButtonState = ButtonState_Base;
			Sprite mBgSprite;
			TextDrawable mLabel;
		};

	}
}