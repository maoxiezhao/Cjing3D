#pragma once

#include "gui\guiInclude.h"
#include "gui\guiWidgets\widgets.h"

#include <optional>

namespace Cjing3D {
namespace Gui {
	
	class InputEventHandler
	{
	public:
		InputEventHandler() = default;

		void HandleInputEvents(const GUIInputEvent& inputEvent, std::vector<WidgetPtr>& widgets);

	private:
		void HandldKeyboardButton(const GUIInputEvent& inputEvent);
		void HandleMouseButton(const GUIInputEvent& inputEvent);
		void HandleMouseMove(const GUIInputEvent& inputEvent);

		void HandleKeyboardButtonEvent(Widget& dispatcher, WidgetPtr targetWidget, const GUIInputEvent& inputEvent);
		void HandleMouseButtonEvent(Widget& dispatcher, WidgetPtr targetWidget, const GUIInputEvent& inputEvent);
	};
}
}