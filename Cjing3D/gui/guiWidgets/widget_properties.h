#pragma once

#include "gui\guiInclude.h"
#include "utils\tinyxml2\tinyxml2.h"

namespace Cjing3D
{
namespace Gui
{
	class Widget;

	class WidgetPropertiesInitializer
	{
	public:
		WidgetPropertiesInitializer();
		~WidgetPropertiesInitializer();

		void InitProperties(Widget& widget, tinyxml2::XMLElement& element);
		void ParseEventHandlers(Widget& widget, tinyxml2::XMLElement& element);
	};
}
}