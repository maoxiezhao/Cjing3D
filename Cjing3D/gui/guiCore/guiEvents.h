#pragma once

#include "input\InputSystem.h"

namespace Cjing3D {
namespace Gui {

	enum class UI_EVENT_TYPE
	{
		UI_EVENT_UNKNOW,

		UI_EVENT_MOUSE_ENTER,
		UI_EVENT_MOUSE_LEAVE,
		UI_EVENT_MOUSE_MOTION,

		UI_EVENT_MOUSE_BUTTON_DOWN,
		UI_EVENT_MOUSE_BUTTON_UP,
		UI_EVENT_MOUSE_BUTTON_CLICK,

		UI_EVENT_KEYBOARD_KEYDOWN,
		UI_EVENT_KEYBOARD_KEYUP,
	};

	enum GUI_INPUT_EVENT_TYPE
	{
		GUI_INPUT_EVENT_TYPE_UNKNOW,
		GUI_INPUT_EVENT_TYPE_KEYBOARD_KEYDOWN,
		GUI_INPUT_EVENT_TYPE_KEYBOARD_KEYUP,
		GUI_INPUT_EVENT_TYPE_MOUSE_MOTION,
		GUI_INPUT_EVENT_TYPE_MOUSE_BUTTONDOWN,
		GUI_INPUT_EVENT_TYPE_MOUSE_BUTTONUP,
	};

	enum GUI_INPUT_KEY_STATE
	{
		GUI_INPUT_KEY_STATE_KEYDOWN,
		GUI_INPUT_KEY_STATE_KEYUP,
	};

	struct GUIInputEvent
	{
		GUI_INPUT_EVENT_TYPE type = GUI_INPUT_EVENT_TYPE_UNKNOW;
		KeyCode key;
		I32x2 pos;
	};
}
}