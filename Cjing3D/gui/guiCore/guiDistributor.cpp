#include "guiDistributor.h"
#include "core\systemContext.hpp"

namespace Cjing3D {
namespace Gui {
	namespace {

		const F32 mouseClickDeltaTime = 0.2f;

		Widget* FindWidget(const I32x2& pos, std::vector<WidgetPtr>& widgets)
		{
			F32x2 fPos = { (F32)pos[0], (F32)pos[1] };
			for (const auto& widget : widgets) 
			{
				if ((widget == nullptr) || !widget->IsVisible()) {
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

		bool BaseDistributor::FireMouseDragEvent(Widget* targetWidget, UI_EVENT_TYPE eventType, const I32x2& offset)
		{
			if (targetWidget == nullptr) {
				return false;
			}

			VariantArray variants;
			variants.push_back(Variant((I32)offset[0]));
			variants.push_back(Variant((I32)offset[1]));

			auto parent = targetWidget->GetRoot();
			if (parent != nullptr) {
				return parent->Fire(UI_EVENT_TYPE::UI_EVENT_MOUSE_DRAG, targetWidget, variants);
			}
			else {
				return targetWidget->Fire(UI_EVENT_TYPE::UI_EVENT_MOUSE_DRAG, targetWidget, variants);
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
				FireMouseDragEvent(mDragWidget.get(), UI_EVENT_TYPE::UI_EVENT_MOUSE_DRAG, pos - mCoords);
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
			if (mMouseFocusWidget != nullptr) {
				mMouseFocusWidget->SetFocused(false);
			}

			mMouseFocusWidget = widget;

			if (widget != nullptr) {
				widget->SetFocused(true);
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
				FireMouseEvent(mButtonFocuseWidget, UI_EVENT_TYPE::UI_EVENT_MOUSE_BUTTON_DOWN, mCoords);
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
					FireMouseEvent(targetWidget, UI_EVENT_TYPE::UI_EVENT_MOUSE_BUTTON_DOWN, mCoords);
				}

				mButtonFocuseWidget = targetWidget;		
			}

			mLastClickWidget = mButtonFocuseWidget;
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
				FireMouseEvent(mButtonFocuseWidget, UI_EVENT_TYPE::UI_EVENT_MOUSE_BUTTON_UP, mCoords);
			}

			Widget* targetWidget = FindAndGetMouseEnableWidget(coords, widgets);
			// handle drag widget
			if (mDragWidget != nullptr)
			{
				if (mDragWidget.get() == targetWidget)
				{
					MouseButtonClick(targetWidget);
				}
				else
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
					MouseButtonClick(targetWidget);
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
				MouseButtonClick(targetWidget);
			}

			mButtonFocuseWidget = nullptr;
		}

		void MouseButtonDistributor::SetCurrentButtonIndex(U32 buttonIndex)
		{
			mCurrentButtonIndex = buttonIndex;
		}

		void MouseButtonDistributor::MouseButtonClick(Widget* widget)
		{
			F32 currentTime = GlobalGetCurrentTime();
			if (mLastPressedTime + mouseClickDeltaTime >= currentTime &&
				widget == mLastClickWidget)
			{
				mLastClickTime = currentTime;
				mLastPressedTime = 0;

				FireMouseButtonEvent(widget, UI_EVENT_TYPE::UI_EVENT_MOUSE_BUTTON_CLICK, mCoords, mCurrentButtonIndex);
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
			HandldKeyboardButton(event.key, GUI_INPUT_KEY_STATE_KEYDOWN, widgets);
			break;
		case GUI_INPUT_EVENT_TYPE_KEYBOARD_KEYUP:
			HandldKeyboardButton(event.key, GUI_INPUT_KEY_STATE_KEYUP, widgets);
			break;
		case GUI_INPUT_EVENT_TYPE_MOUSE_MOTION:
			HandleMouseMove(event.pos, widgets);
			break;
		case GUI_INPUT_EVENT_TYPE_MOUSE_BUTTONDOWN:
			HandleMouseButton(event.pos, event.key, GUI_INPUT_KEY_STATE_KEYDOWN, widgets);
			break;
		case GUI_INPUT_EVENT_TYPE_MOUSE_BUTTONUP:
			HandleMouseButton(event.pos, event.key, GUI_INPUT_KEY_STATE_KEYUP, widgets);
			break;
		default:
			break;
		}
	}

	void EventDistributor::HandldKeyboardButton(KeyCode key, GUI_INPUT_KEY_STATE state, std::vector<WidgetPtr>& widgets)
	{

	}

	void EventDistributor::HandleMouseButton(I32x2 mousePos, KeyCode key, GUI_INPUT_KEY_STATE state, std::vector<WidgetPtr>& widgets)
	{
		SetCurrentButtonIndex((U32)key - (U32)Click_Left);
		if (state == GUI_INPUT_KEY_STATE_KEYDOWN)
		{
			SignalHandlerButtonDown(mousePos, widgets);
		}
		else
		{
			SignalHandlerButtonUp(mousePos, widgets);
		}
	}

	void EventDistributor::HandleMouseMove(I32x2 mousePos, std::vector<WidgetPtr>& widgets)
	{
		SignalHandleMouseMotion(mousePos, widgets);
	}
}
}
