#pragma once

#include "utils\string\utf8String.h"

namespace Cjing3D
{
	struct WindowCloseEvent {};

	struct InputTextEvent
	{
		UTF8String mInputText;
	};
}