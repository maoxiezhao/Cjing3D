#include "inputField.h"
#include "widgetInclude.h"

namespace Cjing3D
{
namespace Gui
{
	F32 TextFieldBoardWidth = 1.0f;

	TextField::TextField(GUIStage& stage, const StringID& name) :
		Widget(stage, name)
	{
		SetSize({ 60.0f, 24.0f });
		SetMargin({ 2.0f, 2.0f, 2.0f, 2.0f });

		// init scroll bar
		mTextEdit = std::make_shared<TextEdit>(stage, "TextEdit");
		mTextEdit->SetSize(GetSize());	
		Widget::Add(mTextEdit);
	}

	void TextField::SetMargin(const WidgetMargin& margin)
	{
		mMargin = margin;
	}

	void TextField::SetText(const UTF8String& text)
	{
		mTextEdit->SetText(text);
	}

	void TextField::SetPlaceholderText(const UTF8String& text)
	{
		mTextEdit->SetPlaceholderText(text);
	}

	void TextField::SetTextColor(const Color4& color)
	{
		mTextEdit->SetTextColor(color);
	}

	void TextField::SetTextSize(U32 size)
	{
		mTextEdit->SetTextSize(size);
	}

	void TextField::SetTextStyle(const std::string& name)
	{
		mTextEdit->SetTextStyle(name);
	}

	void TextField::SetMaxLength(I32 maxLength)
	{
		mTextEdit->SetMaxLength(maxLength);
	}

	void TextField::SetRegexString(const std::string& regexString)
	{
		mTextEdit->SetRegexString(regexString);
	}

	const UTF8String& TextField::GetText() const
	{
		return mTextEdit->GetText();
	}

	void TextField::UpdateLayout()
	{
		F32 width = GetWidth() - mMargin.left - mMargin.right;
		F32 height = GetHeight() - mMargin.top - mMargin.bottom;
		mTextEdit->SetPos({ mMargin.left, mMargin.top });
		mTextEdit->SetSize({ width, height });
	}

	void TextField::RenderImpl(const Rect& destRect)
	{
		GUIRenderer& renderer = GetGUIRenderer();
		const GUIScheme& scheme = renderer.GetGUIScheme();

		F32x2 pos = destRect.GetPos();
		F32x2 size = destRect.GetSize();

		mBorderSprite.SetPos(pos);
		mBorderSprite.SetSize(size);
		mBorderSprite.SetColor(scheme.GetColor(GUIScheme::TextFieldBorder));
		renderer.RenderSprite(mBorderSprite);

		mFieldSprite.SetPos({
			pos[0] + TextFieldBoardWidth,
			pos[1] + TextFieldBoardWidth
		});
		mFieldSprite.SetSize({ 
			size[0] - TextFieldBoardWidth * 2.0f,
			size[1] - TextFieldBoardWidth * 2.0f
		});
		mFieldSprite.SetColor(scheme.GetColor(GUIScheme::TextFieldBackground));
		renderer.RenderSprite(mFieldSprite);
	}
}
}