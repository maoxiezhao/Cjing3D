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

			Button(GUIStage& stage, const StringID& name = StringID::EMPTY, const UTF8String& text = "");

			void SetText(const UTF8String& text);
			void SetTextAlignment(Font::TextAlignH align);
			UTF8String GetText()const;
			TextDrawable& GetButtonLable();

			Signal<void()> OnClickCallback;

		protected:
			virtual void RenderImpl(const Rect& destRect);

			virtual void OnMouseEnter(const VariantArray& variants);
			virtual void OnMouseLeave(const VariantArray& variants);
			virtual void OnMousePressed(const VariantArray& variants);
			virtual void OnMouseReleased(const VariantArray& variants);
			virtual void OnMouseClick(const VariantArray& variants);

		private:
			U32 mButtonState = ButtonState_Base;
			Sprite mBgSprite;
			TextDrawable mLabel;
		};

	}
}