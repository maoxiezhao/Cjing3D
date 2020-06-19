#include "guiEventHandler.h"

namespace Cjing3D 
{
namespace Gui 
{

	void InputEventHandler::HandleInputEvents(const GUIInputEvent& inputEvent, std::vector<WidgetPtr>& widgets)
	{
		switch (inputEvent.type)
		{
		case GUIInputEventType_KEYBOARD_KEYDOWN:
			HandldKeyboardButton(inputEvent);
			break;
		case GUIInputEventType_KEYBOARD_KEYUP:
			HandldKeyboardButton(e.key, GUI_INPUT_KEY_STATE_KEYUP);
			break;
		case GUIInputEventType_MOUSE_MOTION:
			HandleMouseMove(e.pos);
			break;
		case GUIInputEventType_MOUSE_BUTTONDOWN:
			HandleMouseButton(e.key, GUI_INPUT_KEY_STATE_KEYDOWN);
			break;
		case GUIInputEventType_MOUSE_BUTTONUP:
			HandleMouseButton(e.key, GUI_INPUT_KEY_STATE_KEYUP);
			break;
		default:
			break;
		}
	}
	void InputEventHandler::HandldKeyboardButton(const GUIInputEvent& inputEvent)
	{
	}
	void InputEventHandler::HandleMouseButton(const GUIInputEvent& inputEvent)
	{
	}
	void InputEventHandler::HandleMouseMove(const GUIInputEvent& inputEvent)
	{
	}
	void InputEventHandler::HandleKeyboardButtonEvent(Widget& dispatcher, WidgetPtr targetWidget, const GUIInputEvent& inputEvent)
	{
	}
	void InputEventHandler::HandleMouseButtonEvent(Widget& dispatcher, WidgetPtr targetWidget, const GUIInputEvent& inputEvent)
	{
	}
}
}