#pragma once

#include "gui\guiWidgets\widgets.h"

namespace Cjing3D {
namespace Gui {

	class Layout 
	{
	public:
		virtual void UpdateLayout(Widget* widget) = 0;
		virtual F32x2 CalculateBestSize(const Widget* widget) = 0;
	};
}
}