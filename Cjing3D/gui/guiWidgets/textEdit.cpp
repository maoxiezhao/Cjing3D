#include "textEdit.h"
#include "widgetInclude.h"

#include <regex>

namespace Cjing3D
{
namespace Gui
{
	namespace {
		const U32 UTF8CodeBackspace = 0x08;
		const U32 UTF8CodeDelete = 0x7F;
		const U32 UTF8CodeReturn = 0x0D;	

		constexpr F32 CursorHeightMargin = 2.0f;
		constexpr F32 CursorBlinkInterval = 1.0f;

		Color4 selectionTextColor = Color4(220, 220, 220, 160);

		void DeleteText(UTF8String& string, I32& startPos, I32& endPos)
		{
			if (string.Empty()) {
				return;
			}

			I32 start = std::min(startPos, endPos);
			I32 end = std::max(startPos, endPos);
			if (start < 0 || end < 0 || end > string.Length()) {
				return;
			}

			if (start == end)
			{
				// 未有选中文字，则删除当前光标前一个文字
				if (start > 0)
				{
					start = end - 1;
					string.Erase(start, size_t(end) - size_t(start));
				}
			}
			else
			{
				// 表示有选中文字，直接删除
				string.Erase(start, size_t(end) - size_t(start));
			}
			startPos = endPos = start;
		}

		F32x2 CalculateTextPosByIndex(const UTF8String& string, I32 index, Font::FontParams& params)
		{
			if (string.Empty()) {
				return F32x2(0.1f, CursorHeightMargin);
			}

			UTF8String subStr = string.Substr(0, index);
			if (subStr.Empty()) {
				return F32x2(0.1f, CursorHeightMargin);
			}

			return {
				Font::GetTextWidth(subStr, params) + 0.1f,
				CursorHeightMargin
			};
		}

		enum class CursorMoveOperation {
			Left,
			Right,
		};

		bool MoveCursor(CursorMoveOperation operation, const UTF8String& text, I32& startPos, I32& endPos, bool isSelecting)
		{
			if (endPos < 0) {
				return false;
			}

			I32 newPos = endPos;
			switch (operation)
			{
			case CursorMoveOperation::Left:
				newPos = newPos > 0 ? newPos - 1 : 0;
				break;
			case CursorMoveOperation::Right:
				newPos = newPos < text.Length() ? newPos + 1 : newPos;
				break;
			default:
				break;
			}

			if (newPos == endPos) {
				return false;
			}

			if (isSelecting)
			{
				if ((startPos < 0 || startPos == endPos)) 
				{
					startPos = endPos;
				}
				endPos = newPos;
			}
			else
			{
				if (startPos != endPos) {
					newPos = (operation == CursorMoveOperation::Left) ?
						std::min(startPos, endPos) :
						std::max(startPos, endPos);
				}
				startPos = endPos = newPos;
			}
			return true;
		}
	}

	TextEdit::TextEdit(GUIStage& stage, const StringID& name) :
		Widget(stage)
	{
		SetSize({ 60.0f, 22.0f });

		auto& scheme = stage.GetGUIRenderer().GetGUIScheme();
		mTextDrawable.SetColor(scheme.GetColor(GUIScheme::ButtonTextBase));
		mBgSprite.SetColor(scheme.GetColor(GUIScheme::PanelBackground));

		ConnectSignal(UI_EVENT_TYPE::UI_EVENT_MOUSE_BUTTON_DOWN, std::bind(&TextEdit::OnMousePressed, this, std::placeholders::_4), Dispatcher::back_child);
		ConnectSignal(UI_EVENT_TYPE::UI_EVENT_MOUSE_DRAG, std::bind(&TextEdit::OnMouseDrag, this, std::placeholders::_3, std::placeholders::_4), Dispatcher::back_child);
	}

	void TextEdit::SetText(const UTF8String& text)
	{
		if (mText != text)
		{
			mText = text;
			OnTextChanged(text);
		}
	}

	void TextEdit::SetPlaceholderText(const UTF8String& text)
	{
		mPlaceholderText = text;
	}

	void TextEdit::SetTextColor(const Color4& color)
	{
		mTextDrawable.SetColor(color);
	}

	void TextEdit::SetBackgroundColor(const Color4& color)
	{
		mBgSprite.SetColor(color);
	}

	void TextEdit::SetTextSize(U32 size)
	{
		mTextDrawable.SetFontSize(size);
	}

	void TextEdit::SetTextStyle(const std::string& name)
	{
		mTextDrawable.SetFontStyle(name);
	}

	bool TextEdit::HasSelectedText() const
	{
		return mSelectStartPos >= 0 && 
			   mCursorPos >= 0 &&
			   abs(mCursorPos - mSelectStartPos) > 0;
	}

	void TextEdit::Deselect()
	{
		mSelectStartPos = mCursorPos;
	}

	void TextEdit::OnFocusd()
	{
		if (mSelectStartPos < 0) {
			mSelectStartPos = mCursorPos = 0;
		}
	}

	void TextEdit::OnUnFocusd()
	{
		mSelectStartPos = mCursorPos = -1;
	}

	void TextEdit::OnTextInput(const UTF8String& text)
	{
		if (mCursorPos < 0) {
			mCursorPos = 0;
		}
		mCursorPos = std::clamp(mCursorPos, 0, (I32)mText.Length());

		bool isTextChanged = false;
		auto utf8CodePoints =  text.GetCodePoints();
		for (const auto& code : utf8CodePoints)
		{
			// 用于派生对象处理textCode
			if (!PrecessInputTextCode(code)) {
				continue;
			}

			switch (code)
			{
			case UTF8CodeBackspace:
			case UTF8CodeDelete:
				if (!IsReadOnly())
				{
					DeleteText(mText, mSelectStartPos, mCursorPos);
					isTextChanged = true;
				}
				break;
			case UTF8CodeReturn:
				break;
			default:
				{
					// 仅处理可显示字符（code > 31)
					if (!IsReadOnly() && code > 31)
					{
						// 检测最大文本限制
						if (mMaxTextLength >= 0 && mMaxTextLength <= mText.Length()) {
							break;
						}

						// check regex
						if (!mRegexString.empty() && std::regex_search(UTF8String(code).C_Str(), std::regex(mRegexString))) {
							break;
		
						}

						// 如果有选中的文字，则先删除，再写入
						if (HasSelectedText()) 
						{
							DeleteText(mText, mSelectStartPos, mCursorPos);
							Deselect();
						}

						mText.Insert(mCursorPos++, code);
						mSelectStartPos = mCursorPos;
						isTextChanged = true;
					}
				}
				break;
			}
		}

		if (isTextChanged) {
			OnTextChanged(mText);
		}
	}

	void TextEdit::OnKeyDown(KeyCode key, int mod)
	{
		switch (key)
		{
		case Cjing3D::Arrow_Left:
			if (mCursorPos >= 0)
			{
				if (MoveCursor(CursorMoveOperation::Left, mText, mSelectStartPos, mCursorPos, mod & mod_shift)) {
					mCurrentCursorBlinkDuration = 0;
				}
			}
			break;
		case Cjing3D::Arrow_Right:
			if (mCursorPos >= 0)
			{
				if (MoveCursor(CursorMoveOperation::Right, mText, mSelectStartPos, mCursorPos, mod & mod_shift)) {
					mCurrentCursorBlinkDuration = 0;
				}
			}
			break;
		case Cjing3D::Arrow_Up:
			break;
		case Cjing3D::Arrow_Down:
			break;
		default:
			break;
		}
	}

	void TextEdit::OnMousePressed(const VariantArray& variants)
	{
		U32 buttonIndex = variants[2].GetValue<I32>();
		if (buttonIndex != 0) {
			return;
		}

		if (mText.Empty())
		{
			Deselect();
			return;
		}

		F32x2 pos = {
			(F32)variants[0].GetValue<I32>(),
			(F32)variants[1].GetValue<I32>()
		};
		F32x2 localPos = TransformToLocalCoord(pos);
		I32 newCursorPos = mCursorPos;

		// 依次当前行所有文本，取出每个文字的rect做相交判断
		auto textSizeRects = Font::GetTextWidths(mText, mTextDrawable.GetParams());
		size_t index = 0;
		F32 prevWidth = 0.0f;
		for (const auto& width : textSizeRects)
		{
			if ((width + prevWidth) * 0.5f > localPos.x()) {
				break;
			}
			prevWidth = width;
			newCursorPos = ++index;
		}

		mCursorPos = newCursorPos;

		auto& inputManager = GlobalGetSubSystem<InputManager>();
		if (!inputManager.IsKeyHold(KeyCode::Shift_Left) &&
			!inputManager.IsKeyHold(KeyCode::Shift_Right)) {	
			Deselect();
		}
	}

	void TextEdit::OnMouseDrag(bool& handle, const VariantArray& variants)
	{
		F32x2 pos = {
			(F32)variants[2].GetValue<I32>(),
			(F32)variants[3].GetValue<I32>()
		};
		F32x2 localPos = TransformToLocalCoord(pos);
		I32 newCursorPos = mCursorPos;

		// 依次当前行所有文本，取出每个文字的rect做相交判断
		auto textSizeRects = Font::GetTextWidths(mText, mTextDrawable.GetParams());
		size_t index = 0;
		F32 prevWidth = 0.0f;
		for (const auto& width : textSizeRects)
		{
			if ((width + prevWidth) * 0.5f > localPos.x()) {
				break;
			}
			prevWidth = width;
			newCursorPos = ++index;
		}
		
		// 如果未选中状态，则让selectStart为光标当前位置，光标位置则改为对应鼠标位置
		if (mSelectStartPos < 0 && newCursorPos != mCursorPos) {
			mSelectStartPos = mCursorPos;
		}

		mCursorPos = newCursorPos;
		handle = true;
	}

	bool TextEdit::PrecessInputTextCode(I32 code)
	{
		return true;
	}

	void TextEdit::UpdateImpl(F32 dt)
	{
		if (mCursorPos < 0) {
			return;
		}

		mCurrentCursorBlinkDuration += dt;
		if (mCurrentCursorBlinkDuration >= CursorBlinkInterval) {
			mCurrentCursorBlinkDuration = 0;
		}
	}

	void TextEdit::FixedUpdateImpl()
	{
	}

	void TextEdit::RenderImpl(const Rect& destRect)
	{
		GUIRenderer& renderer = GetGUIRenderer();
		const GUIScheme& scheme = renderer.GetGUIScheme();

		renderer.ApplyScissor(destRect);

		// render bg
		if (mBgSprite.GetColor().GetA() > 0)
		{
			mBgSprite.SetPos(destRect.GetPos());
			mBgSprite.SetSize(destRect.GetSize());
			renderer.RenderSprite(mBgSprite);
		}

		// render cursor
		if (!HasSelectedText() && mCursorPos >= 0 && (mCurrentCursorBlinkDuration / CursorBlinkInterval) <= 0.5f)
		{
			F32x2 cursorPos = CalculateTextPosByIndex(mText, mCursorPos, mTextDrawable.GetParams());		
			mCursorSprite.SetPos(destRect.GetPos() + cursorPos);
			mCursorSprite.SetSize({mCursorWidth, mTextDrawable.GetParams().GetLineHeight() - CursorHeightMargin  * 2.0f});
			mCursorSprite.SetColor(Color4::White());
			renderer.RenderSprite(mCursorSprite);
		}
		else if (HasSelectedText())
		{
			I32 start = std::min(mSelectStartPos, mCursorPos);
			I32 end = std::max(mSelectStartPos, mCursorPos);
			if (start < 0 || end < 0 || end - start <= 0) {
				return;
			}

			F32x2 startPos = CalculateTextPosByIndex(mText, start, mTextDrawable.GetParams());
			F32x2 endPos   = CalculateTextPosByIndex(mText, end, mTextDrawable.GetParams());

			mCursorSprite.SetPos(destRect.GetPos() + startPos);
			mCursorSprite.SetSize({(endPos - startPos)[0], mTextDrawable.GetParams().GetLineHeight() - CursorHeightMargin * 2.0f });
			mCursorSprite.SetColor(selectionTextColor);
			renderer.RenderSprite(mCursorSprite);
		}

		// render text
		if (!mText.Empty())
		{
			mTextDrawable.SetText(mText);
			mTextDrawable.SetPos(destRect.GetPos());
			renderer.RenderText(&mTextDrawable);
		}
		else if (!mPlaceholderText.Empty())
		{
			mTextDrawable.SetText(mPlaceholderText);
			mTextDrawable.SetPos(destRect.GetPos());
			renderer.RenderText(&mTextDrawable);
		}

		renderer.ResetScissor();
	}
}
}