#pragma once

#include "gui\guiWidgets\widgets.h"
#include "gui\guiWidgets\textEdit.h"

namespace Cjing3D {
namespace Gui {

	class TextField : public Widget
	{
	public:
		TextField(GUIStage& stage, const StringID& name = StringID::EMPTY);

		void SetMargin(const WidgetMargin& margin);
		void SetText(const UTF8String& text);
		void SetPlaceholderText(const UTF8String& text);
		void SetTextColor(const Color4& color);
		void SetTextSize(U32 size);
		void SetTextStyle(const std::string& name);
		void SetMaxLength(I32 maxLength);
		void SetRegexString(const std::string& regexString);
		const UTF8String& GetText()const;

	protected:
		virtual void UpdateLayoutImpl(const Rect& destRect);
		virtual void RenderImpl(const Rect& destRect);

	private:
		WidgetMargin mMargin;
		Sprite mBorderSprite;
		Sprite mFieldSprite;
		std::shared_ptr<TextEdit> mTextEdit = nullptr;
	};
}
}