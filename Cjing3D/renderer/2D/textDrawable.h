#pragma once

#include "renderer\2D\font.h"

namespace Cjing3D
{
	class TextDrawable
	{
	public:
		TextDrawable();
		~TextDrawable();

		virtual void Update(F32 deltaTime);
		virtual void FixedUpdate();

		void Draw();
		void Draw(const F32x2& pos);

		bool IsVisible()const { return mIsVisible; }
		void SetVisible(bool isVisible) { mIsVisible = isVisible; }
		void SetPos(const F32x2& pos);
		void SetColor(const Color4& color);
		void SetColor(const F32x4& color);
		void SetFontSize(U32 size);
		void SetFontSizeScale(U32 size);
		void SetFontStyle(const std::string& name);
		void SetTextAlignV(Font::TextAlignV align);
		void SetTextAlignH(Font::TextAlignH align);
		void SetBoundingSize(F32 w, F32 h);

		Color4 GetColor()const;
		F32 GetTextWidth()const;
		F32 GetTextHeight()const;

		void SetText(const UTF8String& text);
		void SetText(const char* text);
		void SetText(const std::string& text);

		const UTF8String& GetText()const { return mText; }
		UTF8String& GetText() { return mText; }
		Font::FontParams GetParams() { return mFontParams; }

	private:
		UTF8String mText;
		Font::FontParams mFontParams;
		bool mIsVisible = true;
	};
}