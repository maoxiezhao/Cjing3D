#pragma once

#include "utils\string\utf8String.h"

namespace Cjing3D
{
	struct WindowCloseEvent {};
	struct ViewResizeEvent 
	{
		U32 width = 0;
		U32 height = 0;
	};

	struct InputTextEvent
	{
		UTF8String mInputText;
	};
}