#pragma once

#include "gui\guiWidgets\widgets.h"

namespace Cjing3D {
namespace Gui {

	class Label : public Widget
	{
	public:
		Label(GUIStage& stage, const StringID& name = StringID::EMPTY, const UTF8String& text = "");

		void SetColor(const Color4& color);
		void SetColor(const F32x4& color);
		void SetFontSize(U32 size);
		void SetFontSizeScale(U32 size);
		void SetTextAlignV(Font::FontParams::TextAlignV align);
		void SetTextAlignH(Font::FontParams::TextAlignH align);

		void SetText(const UTF8String& text);
		void SetText(const char* text);
		void SetText(const std::string& text);

		const UTF8String& GetText()const;
		UTF8String& GetText();

	protected:
		virtual void RenderImpl(const Rect& destRect);
		virtual void FixedUpdateImpl();

		void OnTextChanged();

	private:
		TextDrawable mTextDrawable;
		bool mIsTextChanged = true;
	};

}
}