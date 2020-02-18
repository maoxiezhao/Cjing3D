#pragma once

#include "common\common.h"
#include "core\systemContext.hpp"
#include "core\subSystem.hpp"
#include "helper\stringID.h"
#include "utils\color.h"
#include "helper\profiler.h"

namespace Cjing3D
{
	enum WidgetType
	{
		WidgetType_Unknown,
		WidgetType_BaseWidget,
		WidgetType_Panel,
	};
}

//#define CJING_GUI_DEBUG