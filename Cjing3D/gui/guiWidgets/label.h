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
		void SetTextAlignV(Font::TextAlignV align);
		void SetTextAlignH(Font::TextAlignH align);
		void SetBoundingSize(F32 w, F32 h);

		void SetText(const UTF8String& text);
		void SetText(const char* text);
		void SetText(const std::string& text);

		const UTF8String& GetText()const;
		UTF8String& GetText();

		virtual F32x2 CalculateBestSize()const;

	protected:
		virtual void RenderImpl(const Rect& destRect);

	private:
		TextDrawable mTextDrawable;
	};

}
}