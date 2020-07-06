#pragma once

#include "gui\guiWidgets\widgets.h"

namespace Cjing3D {
namespace Gui {

	class TextEdit : public Widget
	{
	public:
		TextEdit(GUIStage& stage, const StringID& name = StringID::EMPTY);

		void SetText(const UTF8String& text);
		void SetPlaceholderText(const UTF8String& text);
		void SetTextColor(const Color4& color);
		void SetBackgroundColor(const Color4& color);
		void SetTextSize(U32 size);
		void SetTextStyle(const std::string& name);
		void SetMaxLength(I32 maxLength) { mMaxTextLength = maxLength; }
		void SetRegexString(const std::string& regexString) { mRegexString = regexString; }
		const UTF8String& GetText()const { return mText; }

		bool IsReadOnly()const { return mIsReadOnly; }
		bool HasSelectedText()const;
		void Deselect();

		virtual void OnFocusd();
		virtual void OnUnFocusd();
		virtual void OnTextInput(const UTF8String& text);
		virtual void OnKeyDown(KeyCode key, int mod);
		virtual void OnMousePressed(const VariantArray& variants);
		virtual void OnMouseDrag(bool& handle, const VariantArray& variants);

		Signal<void(const UTF8String & text)> OnTextChanged;

	protected:
		virtual bool PrecessInputTextCode(I32 code);
		virtual void UpdateImpl(F32 dt);
		virtual void FixedUpdateImpl();
		virtual void RenderImpl(const Rect& destRect);

	private:
		UTF8String mText;
		UTF8String mPlaceholderText;
		bool mIsReadOnly = false;
		I32 mMaxTextLength = -1;
		F32 mCursorWidth = 1.0f;
		WidgetMargin mTextMargion;
		std::string mRegexString;

		I32 mSelectStartPos = -1;
		I32 mCursorPos = -1;
		F32 mCurrentCursorBlinkDuration = 0.0f;

		TextDrawable mTextDrawable;
		Sprite mBgSprite;
		Sprite mCursorSprite;
	};

}
}