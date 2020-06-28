#include "gui\guiWidgets\window.h"
#include "gui\guiWidgets\widgetInclude.h"

namespace Cjing3D {
namespace Gui
{
	Window::Window(GUIStage& stage, const StringID& name, const UTF8String& title, F32x2 fixedSize) :
		Widget(stage, name)
	{
		SetSize(fixedSize);
		SetFixedSize(fixedSize);
		mTitleText.SetText(title);
		mTitleText.SetFontSize(18);
		mTitleText.SetTextAlignH(Font::TextAlignH_Center);

		ConnectSignal(UI_EVENT_TYPE::UI_EVENT_MOUSE_DRAG, std::bind(&Window::OnMouseDrag, this, std::placeholders::_4), Dispatcher::back_child);
	}

	void Window::SetTitle(const UTF8String& text)
	{
		mTitleText.SetText(text);
	}

	const UTF8String& Window::GetTitle() const
	{
		return mTitleText.GetText();
	}

	F32x2 Window::CalculateBestSize() const
	{
		F32x2 bestSize = Widget::CalculateBestSize();
		F32x2 fontSize = { mTitleText.GetTextWidth(), mTitleText.GetTextHeight() };
		return F32x2Max(bestSize, fontSize);
	}

	void Window::OnMouseDrag(const VariantArray& variants)
	{
		F32 offsetX = (F32)variants[0].GetValue<I32>();
		F32 offsetY = (F32)variants[1].GetValue<I32>();
		F32x2 currentPos = GetPos();
		currentPos[0] += offsetX;
		currentPos[1] += offsetY;

		SetPos(currentPos);
	}

	void Window::RenderImpl(const Rect& destRect)
	{
		GUIRenderer& renderer = GetGUIRenderer();
		const GUIScheme& scheme = renderer.GetGUIScheme();

		Color4 backgroundColor;
		Color4 textColor;
		if (IsFocused())
		{
			backgroundColor = scheme.GetColor(GUIScheme::WindowBackgoundFocused);
			textColor = scheme.GetColor(GUIScheme::WindowTitleTextFocused);
		}
		else
		{
			backgroundColor = scheme.GetColor(GUIScheme::WindowBackgound);
			textColor = scheme.GetColor(GUIScheme::WindowTitleText);
		}

		mBgSprite.SetPos(destRect.GetPos());
		mBgSprite.SetSize(destRect.GetSize());
		mBgSprite.SetColor(backgroundColor);
		renderer.RenderSprite(mBgSprite);

		if (!mTitleText.GetText().Empty())
		{
			const F32 headHeight = 30.0f;
			// head bg
			mHeadSprite.SetPos(destRect.GetPos());
			mHeadSprite.SetSize({ destRect.GetSize()[0], headHeight });
			mHeadSprite.SetColor(scheme.GetColor(GUIScheme::WindowHeadBackground));
			renderer.RenderSprite(mHeadSprite);

			// title text
			const auto horizontalPadding = 4.0f;
			F32 privot = 0.0f;
			F32x2 padding = { 0.0f, horizontalPadding };
			switch (mTitleText.GetParams().mTextAlignH)
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

			mTitleText.SetPos(pos);
			mTitleText.SetColor(textColor);
			renderer.RenderText(&mTitleText);
		}
	}
}
}
