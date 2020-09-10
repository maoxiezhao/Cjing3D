#include "checkBox.h"
#include "widgetInclude.h"

namespace Cjing3D {
namespace Gui {
	const F32 checkBoxBorderSpacing = 3.0f;

	CheckBox::CheckBox(GUIStage& stage, const StringID& name) :
		Widget(stage, name)
	{
		SetSize({ 20.0f, 20.0f });
		ConnectSignal(UI_EVENT_TYPE::UI_EVENT_MOUSE_BUTTON_CLICK, std::bind(&CheckBox::OnMouseClick, this, std::placeholders::_4));
	}

	CheckBox::~CheckBox()
	{
	}

	void CheckBox::OnMouseClick(const VariantArray& variants)
	{
		if (!IsEnabled()) {
			return;
		}
		mIsOn = !IsOn();
		OnToggledCallback(mIsOn);
	}

	void CheckBox::RenderImpl(const Rect& destRect)
	{
		GUIRenderer& renderer = GetGUIRenderer();
		const GUIScheme& scheme = renderer.GetGUIScheme();

		mBgSprite.SetPos(destRect.GetPos());
		mBgSprite.SetSize(destRect.GetSize());
		mBgSprite.SetColor(scheme.GetColor(GUIScheme::CheckBoxBackground));
		renderer.RenderSprite(mBgSprite);

		if (IsOn())
		{
			mBgOnSprite.SetPos(destRect.GetPos() + F32x2(checkBoxBorderSpacing, checkBoxBorderSpacing));
			mBgOnSprite.SetSize(destRect.GetSize() - F32x2(checkBoxBorderSpacing * 2.0f, checkBoxBorderSpacing * 2.0f));
			mBgOnSprite.SetColor(scheme.GetColor(GUIScheme::CheckBoxBackgroundOn));
			renderer.RenderSprite(mBgOnSprite);
		}
	}
}
}