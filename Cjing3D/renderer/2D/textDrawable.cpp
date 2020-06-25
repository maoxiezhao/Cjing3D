#include "textDrawable.h"

namespace Cjing3D
{
	TextDrawable::TextDrawable()
	{
	}

	TextDrawable::~TextDrawable()
	{
	}

	void TextDrawable::Update(F32 deltaTime)
	{
	}

	void TextDrawable::FixedUpdate()
	{
	}

	void TextDrawable::Draw()
	{
		Font::Draw(mText, mFontParams);
	}

	void TextDrawable::Draw(const F32x2& pos)
	{
		mFontParams.mPos[0] = pos[0];
		mFontParams.mPos[1] = pos[1];

		Draw();
	}

	void TextDrawable::SetPos(const F32x2& pos)
	{
		mFontParams.mPos[0] = pos[0];
		mFontParams.mPos[1] = pos[1];
	}

	void TextDrawable::SetColor(const Color4& color)
	{
		mFontParams.mColor = color;
	}

	void TextDrawable::SetColor(const F32x4& color)
	{
		mFontParams.mColor.Convert(color);
	}

	void TextDrawable::SetFontSize(U32 size)
	{
		mFontParams.mFontSize = size;
	}

	void TextDrawable::SetFontSizeScale(U32 size)
	{
		// 尽量不修改fontSize，而是使用Scaling，fontSize需要重新渲染fontGlyph Texture.
		mFontParams.mScale = (F32)size / (F32)mFontParams.mFontSize;
	}

	void TextDrawable::SetTextAlignV(Font::FontParams::TextAlignV align)
	{
		mFontParams.mTextAlignV = align;
	}

	void TextDrawable::SetTextAlignH(Font::FontParams::TextAlignH align)
	{
		mFontParams.mTextAlignH = align;
	}

	// 获取文字宽度性能较差
	F32 TextDrawable::GetTextWidth() const
	{
		return Font::GetTextWidth(mText, mFontParams);
	}

	// 获取文字高度性能较差
	F32 TextDrawable::GetTextHeight() const
	{
		return Font::GetTextHeight(mText, mFontParams);
	}

	void TextDrawable::SetText(const UTF8String& text)
	{
		mText = text;
	}

	void TextDrawable::SetText(const char* text)
	{
		mText = text;
	}

	void TextDrawable::SetText(const std::string& text)
	{
		mText = text;
	}
}
