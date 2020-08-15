#pragma once

#include "common\common.h"
#include "core\globalContext.hpp"
#include "core\subSystem.hpp"
#include "helper\stringID.h"
#include "helper\profiler.h"
#include "helper\enumInfo.h"
#include "utils\color.h"
#include "utils\signal\signal.h"
#include "utils\treeNode.h"
#include "utils\geometry.h"
#include "utils\container\dynamicArray.h"

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