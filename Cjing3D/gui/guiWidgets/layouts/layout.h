#pragma once

#include "gui\guiWidgets\widgets.h"

namespace Cjing3D {
namespace Gui {

	enum AlignmentMode
	{
		AlignmentMode_Begin = 0,
		AlignmentMode_Center,
		AlignmentMode_End,
		AlignmentMode_Fill,
	};
	enum AlignmentOrien
	{
		AlignmentOrien_Horizontal = 0,
		AlignmentOrien_Vertical
	};

	class Layout 
	{
	public:
		virtual void UpdateLayout(Widget* widget) = 0;
		virtual F32x2 CalculateBestSize(const Widget* widget) = 0;
	};
}
}