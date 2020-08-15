#include "guiDistributor.h"
#include "core\globalContext.hpp"

namespace Cjing3D {
namespace Gui {
	namespace {

		const F32 mouseClickDeltaTime = 0.2f;

		Widget* FindWidget(const I32x2& pos, std::vector<WidgetPtr>& widgets)
		{
			F32x2 fPos = { (F32)pos[0], (F32)pos[1] };
			for (const auto& widget : widgets) 
			{
				if ((widget == nullptr) || !widget->IsVisible() || !widget->IsEnabled()) {
					continue;
				}

				if (!widget->HitTest(fPos)) {
					continue;
				}

				auto child = widget->GetChildWidgetByGlobalCoords(fPos);
				if (child != nullptr) {
					return child.get();
				}
			}
			return nullptr;
		}

	}

	namespace Impl
	{
		
		bool BaseDistributor::FireKeyboardEvent(Widget* targetWidget, UI_EVENT_TYPE eventType, KeyCode key)
		{
			return false;
		}

		bool BaseDistributor::FireMouseEvent(Widget* targetWidget, UI_EVENT_TYPE eventType, const I32x2& pos)
		{
#ifdef CJING_GUI_DEBUG
			switch (eventType)
			{
			case Cjing3D::Gui::UI_EVENT_TYPE::UI_EVENT_MOUSE_ENTER:
				Logger::Info("[GUI DEBUG] OnMouseEnter:" + targetWidget->GetName().GetString());
				break;
			case Cjing3D::Gui::UI_EVENT_TYPE::UI_EVENT_MOUSE_LEAVE:
				Logger::Info("[GUI DEBUG] OnMouseLeave:" + targetWidget->GetName().GetString());
				break;
			case Cjing3D::Gui::UI_EVENT_TYPE::UI_EVENT_MOUSE_MOTION:
				Logger::Info("[GUI DEBUG] OnMouseMotion:" + targetWidget->GetName().GetString());
				break;
			}
#endif
			if (targetWidget == nullptr) {
				return false;
			}

			VariantArray variants;
			variants.push_back(Variant((I32)pos[0]));
			variants.push_back(Variant((I32)pos[1]));

			auto parent = targetWidget->GetRoot();
			if (parent != nullptr) {
				return parent->Fire(eventType, targetWidget, variants);
			}
			else {
				return targetWidget->Fire(eventType, targetWidget, variants);
			}
		}

		bool BaseDistributor::FireMouseButtonEvent(Widget* targetWidget, UI_EVENT_TYPE eventType, const I32x2& pos, U32 buttonIndex)
		{
			if (targetWidget == nullptr) {
				return false;
			}

			VariantArray variants;
			variants.push_back(Variant((I32)pos[0]));
			variants.push_back(Variant((I32)pos[1]));
			variants.push_back(Variant((I32)buttonIndex));

			auto parent = targetWidget->GetRoot();
			if (parent != nullptr) {
				return parent->Fire(eventType, targetWidget, variants);
			}
			else {
				return targetWidget->Fire(eventType, targetWidget, variants);
			}
		}

		bool BaseDistributor::FireMouseDragEvent(Widget* targetWidget, UI_EVENT_TYPE eventType, const I32x2& offset, const I32x2& pos)
		{
			if (targetWidget == nullptr) {
				return false;
			}

			VariantArray variants;
			variants.push_back(Variant((I32)offset[0]));
			variants.push_back(Variant((I32)offset[1]));
			variants.push_back(Variant((I32)pos[0]));
			variants.push_back(Variant((I32)pos[1]));

			auto parent = targetWidget->GetRoot();
			if (parent != nullptr) {
				return parent->Fire(UI_EVENT_TYPE::UI_EVENT_MOUSE_DRAG, targetWidget, variants);
			}
			else {
				return targetWidget->Fire(UI_EVENT_TYPE::UI_EVENT_MOUSE_DRAG, targetWidget, variants);
			}
		}

		bool BaseDistributor::FireMouseScrollEvent(Widget* targetWidget, const I32x2& pos, I32 delta)
		{
			if (targetWidget == nullptr) {
				return false;
			}

			VariantArray variants;
			variants.push_back(Variant(pos[0]));
			variants.push_back(Variant(pos[1]));
			variants.push_back(Variant(delta));

			auto parent = targetWidget->GetRoot();
			if (parent != nullptr) {
				return parent->Fire(UI_EVENT_TYPE::UI_EVENT_MOUSE_SCROLL, targetWidget, variants);
			}
			else {
				return targetWidget->Fire(UI_EVENT_TYPE::UI_EVENT_MOUSE_SCROLL, targetWidget, variants);
			}
		}

		Widget* BaseDistributor::FindAndGetMouseEnableWidget(const I32x2& pos, std::vector<WidgetPtr>& widgets)
		{
			Widget* targetWidget = FindWidget(pos, widgets);
			while (targetWidget && !targetWidget->CanMouseFocus() && targetWidget->GetParent())
			{
				targetWidget = targetWidget->GetParent();
			}
			return targetWidget;
		}

		void MouseMotion::OnMouseEnter(Widget* widget)
		{
			if (widget != nullptr)
			{
				SetMouseFocusedWidget(widget->GetNodePtr());
				FireMouseEvent(widget, UI_EVENT_TYPE::UI_EVENT_MOUSE_ENTER, mCoords);
			}
			else
			{
				SetMouseFocusedWidget(nullptr);
			}
		}

		void MouseMotion::OnMouseLeave()
		{
			FireMouseEvent(mMouseFocusWidget.get(), UI_EVENT_TYPE::UI_EVENT_MOUSE_LEAVE, mCoords);
			SetMouseFocusedWidget(nullptr);
		}

		void MouseMotion::OnMouseHover(Widget* widget)
		{
			FireMouseEvent(widget, UI_EVENT_TYPE::UI_EVENT_MOUSE_MOTION, mCoords);
		}

		void MouseMotion::SignalHandleMouseMotion(const I32x2& pos, std::vector<WidgetPtr>& widgets)
		{
			if (mDragWidget != nullptr)
			{
				if (FireMouseDragEvent(mDragWidget.get(), UI_EVENT_TYPE::UI_EVENT_MOUSE_DRAG, pos - mCoords, pos)) {
					mCoords = pos;
					return;
				}
			}

			mCoords = pos;

			if (mMouseCaptured && mMouseFocusWidget != nullptr)
			{
				OnMouseHover(mMouseFocusWidget.get());
			}
			else
			{
				Widget* targetWidget = FindAndGetMouseEnableWidget(pos, widgets);
				if (targetWidget != nullptr) {
					if (FireMouseEvent(targetWidget, UI_EVENT_TYPE::UI_EVENT_MOUSE_MOTION, mCoords)) {
						return;
					}
				}

				if (mMouseFocusWidget == nullptr && targetWidget != nullptr) {
					OnMouseEnter(targetWidget);
				}
				else if (mMouseFocusWidget != nullptr && targetWidget == nullptr)
				{
					OnMouseLeave();
				}
				else if (mMouseFocusWidget && targetWidget == mMouseFocusWidget.get())
				{
					OnMouseHover(targetWidget);
				}
				else if (mMouseFocusWidget && targetWidget)
				{
					OnMouseLeave();
					OnMouseEnter(targetWidget);
				}
			}
		}

		void MouseMotion::SetDragWidget(WidgetPtr widget)
		{
			mDragWidget = widget;
		}

		void MouseMotion::SetMouseFocusedWidget(WidgetPtr widget)
		{
			mMouseFocusWidget = widget;
		}

		void MouseButtonDistributor::SetCurrentFocusedWidget(WidgetPtr widget)
		{
			if (widget != mCurrentFocusedWidget)
			{
				if (mCurrentFocusedWidget != nullptr) {
					mCurrentFocusedWidget->OnUnFocusd();
				}

				OnFocusedChanged(mCurrentFocusedWidget, widget);
				mCurrentFocusedWidget = widget;

				if (widget != nullptr) {
					widget->OnFocusd();
				}
			}
		}

		void MouseButtonDistributor::SignalHandlerButtonDown(const I32x2& coords, std::vector<WidgetPtr>& widgets)
		{
			if (mIsPressed) {
				return;
			}
			mIsPressed = true;

			if (mMouseCaptured && mMouseFocusWidget != nullptr)
			{
				mButtonFocuseWidget = mMouseFocusWidget.get();
				FireMouseButtonEvent(mButtonFocuseWidget, UI_EVENT_TYPE::UI_EVENT_MOUSE_BUTTON_DOWN, coords, mCurrentButtonIndex);
			}
			else
			{
				Widget* targetWidget = FindAndGetMouseEnableWidget(coords, widgets);
				if (mMouseFocusWidget.get() != targetWidget) {
					SetMouseFocusedWidget(targetWidget != nullptr ? targetWidget->GetNodePtr() : nullptr);
				}

				if (mDragWidget == nullptr && targetWidget != nullptr) {
					mDragWidget = targetWidget->GetNodePtr();
				}

				if (targetWidget != nullptr) {
					FireMouseButtonEvent(targetWidget, UI_EVENT_TYPE::UI_EVENT_MOUSE_BUTTON_DOWN, coords, mCurrentButtonIndex);
				}

				mButtonFocuseWidget = targetWidget;		
			}

			mLastClickWidget = mButtonFocuseWidget != nullptr ? mButtonFocuseWidget->GetNodePtr() : nullptr;
			SetCurrentFocusedWidget(mLastClickWidget);
			mLastPressedTime = GlobalGetCurrentTime();
		}

		void MouseButtonDistributor::SignalHandlerButtonUp(const I32x2& coords, std::vector<WidgetPtr>& widgets)
		{
			if (!mIsPressed) {
				return;
			}
			mIsPressed = false;

			// handle button focused
			if (mButtonFocuseWidget != nullptr) {
				FireMouseButtonEvent(mButtonFocuseWidget, UI_EVENT_TYPE::UI_EVENT_MOUSE_BUTTON_UP, coords, mCurrentButtonIndex);
			}

			Widget* targetWidget = FindAndGetMouseEnableWidget(coords, widgets);
			// handle drag widget
			if (mDragWidget != nullptr)
			{
				if (mDragWidget.get() != targetWidget)
				{
					OnMouseLeave();
					if (targetWidget != nullptr) {
						OnMouseEnter(targetWidget);
					}
				}
				SetDragWidget(nullptr);
			}

			// handle mouse captured
			if (mMouseCaptured )
			{
				SetMouseCaptured(false);
				if (mMouseFocusWidget.get() == targetWidget)
				{
					MouseButtonClick(targetWidget, coords);
				}
				else
				{
					OnMouseLeave();
					if (targetWidget != nullptr) {
						OnMouseEnter(targetWidget);
					}
				}
			}
			else if (mButtonFocuseWidget != nullptr && mButtonFocuseWidget == targetWidget)
			{
				MouseButtonClick(targetWidget, coords);
			}
	
			mButtonFocuseWidget = nullptr;
		}

		void MouseButtonDistributor::SignalHandlerMouseWheelChanged(I32 wheelDelta, const I32x2& coords, std::vector<WidgetPtr>& widgets)
		{
			if (wheelDelta == 0) {
				return;
			}
		
			if (mMouseCaptured && mMouseFocusWidget != nullptr)
			{
				FireMouseScrollEvent(mMouseFocusWidget.get(), coords, wheelDelta);
			}
			else
			{
				Widget* targetWidget = FindAndGetMouseEnableWidget(coords, widgets);
				if (targetWidget != nullptr) {
					FireMouseScrollEvent(targetWidget, coords, wheelDelta);
				}
			}
		}

		void MouseButtonDistributor::SetCurrentButtonIndex(U32 buttonIndex)
		{
			mCurrentButtonIndex = buttonIndex;
		}

		void MouseButtonDistributor::MouseButtonClick(Widget* widget, const I32x2& coords)
		{
			F32 currentTime = GlobalGetCurrentTime();
			if (mLastPressedTime + mouseClickDeltaTime >= currentTime &&
				widget == mLastClickWidget.get())
			{
				mLastClickTime = currentTime;
				mLastPressedTime = 0;

				FireMouseButtonEvent(widget, UI_EVENT_TYPE::UI_EVENT_MOUSE_BUTTON_CLICK, coords, mCurrentButtonIndex);
			}
		}
	}

	EventDistributor::EventDistributor()
	{
	}

	void EventDistributor::HandleInpueEvent(const GUIInputEvent& event, std::vector<WidgetPtr>& widgets)
	{
		switch (event.type)
		{
		case GUI_INPUT_EVENT_TYPE_KEYBOARD_KEYDOWN:
			HandldKeyboardButton(event.key, event.modifiers, GUI_INPUT_KEY_STATE_KEYDOWN, widgets);
			break;
		case GUI_INPUT_EVENT_TYPE_KEYBOARD_KEYUP:
			HandldKeyboardButton(event.key, event.modifiers, GUI_INPUT_KEY_STATE_KEYUP, widgets);
			break;
		case GUI_INPUT_EVENT_TYPE_MOUSE_MOTION:
			SignalHandleMouseMotion(event.pos, widgets);
			break;
		case GUI_INPUT_EVENT_TYPE_MOUSE_BUTTONDOWN:
			SetCurrentButtonIndex((U32)event.key - (U32)Click_Left);
			SignalHandlerButtonDown(event.pos, widgets);
			break;
		case GUI_INPUT_EVENT_TYPE_MOUSE_BUTTONUP:
			SetCurrentButtonIndex((U32)event.key - (U32)Click_Left);
			SignalHandlerButtonUp(event.pos, widgets);
			break;
		case GUI_INPUT_EVENT_TYPE_MOUSE_WHEEL_CHANGED:
			SignalHandlerMouseWheelChanged(event.delta, event.pos, widgets);
			break;
		case GUI_INPUT_EVENT_TYPE_INPUT_TEXT:
			SignalHandlerInputText(event.inputText, widgets);
			break;
		default:
			break;
		}
	}

	void EventDistributor::SignalHandlerInputText(const UTF8String& inputText, std::vector<WidgetPtr>& widgets)
	{
		if (GetCurrentFocusdWidget() != nullptr && !inputText.Empty()) {
			GetCurrentFocusdWidget()->OnTextInput(inputText);
		}
	}

	void EventDistributor::HandldKeyboardButton(KeyCode key, int mod, GUI_INPUT_KEY_STATE state, std::vector<WidgetPtr>& widgets)
	{
		auto widget = GetCurrentFocusdWidget();
		if (widget == nullptr) {
			return;
		}

		if (state == GUI_INPUT_KEY_STATE_KEYDOWN) {
			widget->OnKeyDown(key, mod);
		}
		else {
			widget->OnKeyUp(key, mod);
		}
	}
}
}
