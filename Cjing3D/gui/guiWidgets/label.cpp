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

	void Label::SetTextAlignV(Font::TextAlignV align)
	{
		mTextDrawable.SetTextAlignV(align);
	}

	void Label::SetTextAlignH(Font::TextAlignH align)
	{
		mTextDrawable.SetTextAlignH(align);
	}

	void Label::SetBoundingSize(F32 w, F32 h)
	{
		Widget::SetFixedSize({ w, h });
		mTextDrawable.SetBoundingSize(w, h);
	}

	void Label::SetText(const UTF8String& text)
	{
		if (text != mTextDrawable.GetText())
		{
			mTextDrawable.SetText(text);
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

	F32x2 Label::CalculateBestSize() const
	{
		F32x2 fixedSize = GetFixedSize();
		if (mTextDrawable.GetText().Empty()) {
			return fixedSize;
		}

		if (fixedSize[0] > 0.0f || fixedSize[1] > 0.0f) {
			if (IsF32EqualZero(fixedSize[1]))
			{
				F32 h = mTextDrawable.GetTextHeight();
				fixedSize[1] = h;
				return fixedSize;
			}
			else
			{
				return fixedSize;
			}
		}

		F32 w = mTextDrawable.GetTextWidth();
		F32 h = mTextDrawable.GetTextHeight();
		return F32x2(w, h);
	}

	void Label::RenderImpl(const Rect& destRect)
	{
		GUIRenderer& renderer = GetGUIRenderer();
		
		F32x2 targetPos = destRect.GetPos();
		switch (mTextDrawable.GetParams().mTextAlignH)
		{
		case Font::TextAlignH_LEFT:
			break;
		case Font::TextAlignH_Center:
			targetPos[0] += GetSize()[0] * 0.5f;
			break;
		case Font::TextAlignH_Right:
			targetPos[0] += GetSize()[0];
			break;
		default:
			break;
		}

		mTextDrawable.SetPos(targetPos);
		renderer.RenderText(&mTextDrawable);
	}
}
}