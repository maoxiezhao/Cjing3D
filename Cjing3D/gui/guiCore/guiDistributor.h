#pragma once

#include "gui\guiInclude.h"
#include "gui\guiWidgets\widgets.h"

namespace Cjing3D {
namespace Gui {

	////////////////////////////////////////////////////////////////////////////////////////
	/// Event params description
	////////////////////////////////////////////////////////////////////////////////////////
	//
	// UI_EVENT_MOUSE_ENTER
	// UI_EVENT_MOUSE_LEAVE
	// UI_EVENT_MOUSE_MOTION
	// params:
	//      variants[1] = mouseCoors[0]
	//      variants[2] = mouseCoors[1]
	//
	// UI_EVENT_MOUSE_BUTTON_DOWN
	// UI_EVENT_MOUSE_BUTTON_UP
	// params:
	//      variants[1] = mouseCoors[0]
	//      variants[2] = mouseCoors[1]
	//      variants[3] = buttonIndex
	//
	// UI_EVENT_MOUSE_DRAG
	// params:
	//      variants[1] = offsetX[0]
	//      variants[2] = offsetY[1]
	//      variants[3] = mouseCoors[0]
	//      variants[4] = mouseCoors[1]
	//
	//	UI_EVENT_MOUSE_SCROLL
	// params:
	//      variants[1] = mouseCoors[0]
	//      variants[2] = mouseCoors[1]
	//      variants[3] = wheelDelta
	//
	////////////////////////////////////////////////////////////////////////////////////////

	namespace Impl 
	{
		class BaseDistributor
		{
		protected:
			bool FireKeyboardEvent(Widget* targetWidget, UI_EVENT_TYPE eventType, KeyCode key);
			bool FireMouseEvent(Widget* targetWidget, UI_EVENT_TYPE eventType, const I32x2& pos);
			bool FireMouseButtonEvent(Widget* targetWidget, UI_EVENT_TYPE eventType, const I32x2& pos, U32 buttonIndex);
			bool FireMouseDragEvent(Widget* targetWidget, UI_EVENT_TYPE eventType, const I32x2& offset, const I32x2& pos);
			bool FireMouseScrollEvent(Widget* targetWidget, const I32x2& pos, I32 delta);

			Widget* FindAndGetMouseEnableWidget(const I32x2& pos, std::vector<WidgetPtr>& widgets);
		};

		class MouseMotion : public BaseDistributor
		{
		public:
			void SetMouseCaptured(bool captrued = true) { mMouseCaptured = captrued; }
			WidgetPtr GetMouseFocusWidget() { return mMouseFocusWidget; }
			WidgetPtr GetMouseDragWidget() { return mDragWidget; }

		protected:
			void OnMouseEnter(Widget* widget);
			void OnMouseLeave();
			void OnMouseHover(Widget* widget);
			void SignalHandleMouseMotion(const I32x2& pos, std::vector<WidgetPtr>& widgets);
			void SetDragWidget(WidgetPtr widget);
			void SetMouseFocusedWidget(WidgetPtr widget);

			WidgetPtr mMouseFocusWidget = nullptr;
			WidgetPtr mDragWidget = nullptr;
			I32x2  mCoords = I32x2(0, 0);
			bool mMouseCaptured = false;
			bool mSignalHandelMouseMotionEntered;
		};

		class MouseButtonDistributor : public MouseMotion
		{
		public:
			WidgetPtr GetCurrentFocusdWidget() { return mCurrentFocusedWidget; }
			void SetCurrentFocusedWidget(WidgetPtr widget);

			Signal<void(WidgetPtr oldWidget, WidgetPtr newWidge)> OnFocusedChanged;

		protected:
			void SignalHandlerButtonDown(const I32x2& coords, std::vector<WidgetPtr>& widgets);
			void SignalHandlerButtonUp(const I32x2& coords, std::vector<WidgetPtr>& widgets);
			void SignalHandlerMouseWheelChanged(I32 wheelDelta, const I32x2& coords, std::vector<WidgetPtr>& widgets);

			void SetCurrentButtonIndex(U32 buttonIndex);
			void SetCanDrag(bool canDrag) { mCanDrag = canDrag; }
		
		private:
			void MouseButtonClick(Widget* widget, const I32x2& coords);

			WidgetPtr mLastClickWidget = nullptr;
			WidgetPtr mCurrentFocusedWidget = nullptr;
			Widget* mButtonFocuseWidget = nullptr;

			bool mIsPressed = false;
			bool mCanDrag = true;
			F32 mLastPressedTime = 0.0f;
			F32 mLastClickTime = 0.0f;
			U32 mCurrentButtonIndex = 0;	// 0-left, 1-middle, 2-right
		};
	}

	class EventDistributor : public Impl::MouseButtonDistributor
	{
	public:
		EventDistributor();

		void HandleInpueEvent(const GUIInputEvent& event, std::vector<WidgetPtr>& widgets);

	private:
		void HandldKeyboardButton(KeyCode key, int mod, GUI_INPUT_KEY_STATE state, std::vector<WidgetPtr>& widgets);
		void SignalHandlerInputText(const UTF8String& inputText, std::vector<WidgetPtr>& widgets);
	};

}
}