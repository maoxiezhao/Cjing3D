#include "widgetInclude.h"
#include "button.h"

namespace Cjing3D {
namespace Gui {

	Button::Button(GUIStage& stage, const StringID& name) :
		Widget(stage, name)
	{
		SetSize({ 74.0f, 24.0f });
		mLabel.SetTextAlignH(Font::TextAlignH_Center);

		ConnectSignal(UI_EVENT_TYPE::UI_EVENT_MOUSE_ENTER, std::bind(&Button::OnMouseEnter, this, std::placeholders::_4));
		ConnectSignal(UI_EVENT_TYPE::UI_EVENT_MOUSE_LEAVE, std::bind(&Button::OnMouseLeave, this, std::placeholders::_4));
		ConnectSignal(UI_EVENT_TYPE::UI_EVENT_MOUSE_BUTTON_DOWN,  std::bind(&Button::OnMousePressed, this, std::placeholders::_4));
		ConnectSignal(UI_EVENT_TYPE::UI_EVENT_MOUSE_BUTTON_UP,    std::bind(&Button::OnMouseReleased, this, std::placeholders::_4));
		ConnectSignal(UI_EVENT_TYPE::UI_EVENT_MOUSE_BUTTON_CLICK, std::bind(&Button::OnMouseClick, this, std::placeholders::_4));
	}

	void Button::SetText(const UTF8String& text)
	{
		mLabel.SetText(text);
	}

	void Button::SetTextAlignment(Font::TextAlignH align)
	{
		mLabel.SetTextAlignH(align);
	}

	UTF8String Button::GetText() const
	{
		return mLabel.GetText();
	}

	TextDrawable& Button::GetButtonLable()
	{
		return mLabel;
	}

	void Button::RenderImpl(const Rect& destRect)
	{
		GUIRenderer& renderer = GetGUIRenderer();
		const GUIScheme& scheme = renderer.GetGUIScheme();

		// render background 
		Color4 backgroundColor = scheme.GetColor(GUIScheme::ButtonBackgroundBase);
		Color4 textColor = scheme.GetColor(GUIScheme::ButtonTextBase);
		if (mButtonState & ButtonState_Pressed)
		{
			backgroundColor = scheme.GetColor(GUIScheme::ButtonBackgroundClick);
			textColor = scheme.GetColor(GUIScheme::ButtonTextClick);
		}
		else if (mButtonState & ButtonState_Hovered)
		{
			backgroundColor = scheme.GetColor(GUIScheme::ButtonBackgroundHovered);
			textColor = scheme.GetColor(GUIScheme::ButtonTextHovered);
		}

		mBgSprite.SetPos(destRect.GetPos());
		mBgSprite.SetSize(destRect.GetSize());
		mBgSprite.SetColor(backgroundColor);
		renderer.RenderSprite(mBgSprite);

		// render text
		if (!mLabel.GetText().Empty())
		{
			const auto baselineHeight = 3.0f;
			const auto horizontalPadding = 4.0f;

			F32 privot = 0.0f;
			F32x2 padding = { 0.0f, horizontalPadding };
			switch (mLabel.GetParams().mTextAlignH)
			{
			case Font::TextAlignH_LEFT:
				padding[0] = horizontalPadding;
				break;
			case Font::TextAlignH_Center:
				privot = 0.5f;
				break;
			case Font::TextAlignH_Right:
				privot = 1.0f;
				padding[0] = -horizontalPadding;
				break;
			}

			F32x2 pos = destRect.GetPos() + padding;
			pos[0] += destRect.GetWidth() * privot;

			mLabel.SetPos(pos);
			mLabel.SetColor(textColor);
			renderer.RenderText(&mLabel);
		}
	}

	void Button::OnMouseEnter(const VariantArray& variants)
	{
		if (!IsEnabled()) {
			return;
		}
		mButtonState |= ButtonState_Hovered;
	}

	void Button::OnMouseLeave(const VariantArray& variants)
	{
		mButtonState &= ~ButtonState_Hovered;
	}

	void Button::OnMousePressed(const VariantArray& variants)
	{
		mButtonState |= ButtonState_Pressed;
	}

	void Button::OnMouseReleased(const VariantArray& variants)
	{
		mButtonState &= ~ButtonState_Pressed;
	}

	void Button::OnMouseClick(const VariantArray& variants)
	{
		if (!IsEnabled()) {
			return;
		}
		OnClickCallback();
	}

}
}