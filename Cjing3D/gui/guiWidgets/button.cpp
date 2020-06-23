#include "stackPanel.h"
#include "widgetInclude.h"
#include "button.h"

namespace Cjing3D {
namespace Gui {

	Button::Button(GUIStage& stage, const StringID& name) :
		Widget(stage, name)
	{
		SetSize({ 74.0f, 24.0f });

		ConnectSignal(UI_EVENT_TYPE::UI_EVENT_MOUSE_ENTER, std::bind(&Button::OnMouseEnter, this, std::placeholders::_4));
		ConnectSignal(UI_EVENT_TYPE::UI_EVENT_MOUSE_LEAVE, std::bind(&Button::OnMouseLeave, this, std::placeholders::_4));
		ConnectSignal(UI_EVENT_TYPE::UI_EVENT_MOUSE_BUTTON_DOWN,  std::bind(&Button::OnMousePressed, this, std::placeholders::_4));
		ConnectSignal(UI_EVENT_TYPE::UI_EVENT_MOUSE_BUTTON_UP,    std::bind(&Button::OnMouseReleased, this, std::placeholders::_4));
		ConnectSignal(UI_EVENT_TYPE::UI_EVENT_MOUSE_BUTTON_CLICK, std::bind(&Button::OnMouseClick, this, std::placeholders::_4));
	}

	Button::~Button()
	{
	}

	void Button::OnLoaded()
	{
	}

	void Button::OnUnloaded()
	{
	}

	void Button::Update(F32 dt)
	{
	}

	void Button::FixedUpdate()
	{
	}

	void Button::RenderImpl(const Rect& destRect)
	{
		GUIRenderer& renderer = GetGUIRenderer();
		const GUIScheme& scheme = renderer.GetGUIScheme();

		Color4 backgroundColor = scheme.GetColor(GUIScheme::ButtonBackgroundBase);
		if (mButtonState & ButtonState_Pressed)
		{
			backgroundColor = scheme.GetColor(GUIScheme::ButtonBackgroundClick);
		}
		else if (mButtonState & ButtonState_Hovered)
		{
			backgroundColor = scheme.GetColor(GUIScheme::ButtonBackgroundHovered);
		}

		mBgSprite.SetPos(destRect.GetPos());
		mBgSprite.SetSize(destRect.GetSize());
		mBgSprite.SetColor(backgroundColor);
		renderer.RenderSprite(mBgSprite);
	}

	void Button::OnMouseEnter(const VariantArray& variants)
	{
		if (!IsEnabled()) {
			return;
		}
		mButtonState |= ButtonState_Hovered;
	}

	void Button::OnMouseLeave(const VariantArray& variants)
	{
		mButtonState &= ~ButtonState_Hovered;
	}

	void Button::OnMousePressed(const VariantArray& variants)
	{
		mButtonState |= ButtonState_Pressed;
	}

	void Button::OnMouseReleased(const VariantArray& variants)
	{
		mButtonState &= ~ButtonState_Pressed;
	}

	void Button::OnMouseClick(const VariantArray& variants)
	{
		if (!IsEnabled()) {
			return;
		}
		mClickCallback();
	}

}
}