#include "label.h"
#include "widgetInclude.h"

namespace Cjing3D{
namespace Gui {

	Label::Label(GUIStage& stage, const StringID& name, const UTF8String& text) :
		Widget(stage, name)
	{
		SetText(text);
	}

	void Label::SetColor(const Color4& color)
	{
		mTextDrawable.SetColor(color);
	}

	void Label::SetColor(const F32x4& color)
	{
		mTextDrawable.SetColor(color);
	}

	void Label::SetFontSize(U32 size)
	{
		mTextDrawable.SetFontSize(size);
	}

	void Label::SetFontSizeScale(U32 size)
	{
		mTextDrawable.SetFontSizeScale(size);
	}

	void Label::SetTextAlignV(Font::FontParams::TextAlignV align)
	{
		mTextDrawable.SetTextAlignV(align);
	}

	void Label::SetTextAlignH(Font::FontParams::TextAlignH align)
	{
		mTextDrawable.SetTextAlignH(align);
	}

	void Label::SetText(const UTF8String& text)
	{
		if (text != mTextDrawable.GetText())
		{
			mTextDrawable.SetText(text);
			mIsTextChanged = true;
		}
	}

	void Label::SetText(const char* text)
	{
		SetText(UTF8String(text));
	}

	void Label::SetText(const std::string& text)
	{
		SetText(UTF8String(text));
	}

	const UTF8String& Label::GetText() const
	{
		return mTextDrawable.GetText();
	}

	UTF8String& Label::GetText()
	{
		return mTextDrawable.GetText();
	}

	void Label::OnTextChanged()
	{
		if (!mIsTextChanged || mTextDrawable.GetText().Empty()) {
			return;
		}

		F32 w = mTextDrawable.GetTextWidth();
		F32 h = mTextDrawable.GetTextHeight();
		if (w <= 0.0f || h <= 0.0f) {
			return;
		}

		SetSize({ w, h });
		mIsTextChanged = true;
	}

	void Label::RenderImpl(const Rect& destRect)
	{
		GUIRenderer& renderer = GetGUIRenderer();
	
		mTextDrawable.SetPos(destRect.GetPos());
		renderer.RenderText(&mTextDrawable);
	}

	void Label::FixedUpdateImpl()
	{
		OnTextChanged();
	}
}
}