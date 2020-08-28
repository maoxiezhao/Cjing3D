#pragma once

#include "platform\win32\includeWin32.h"
#include "input\keyboard.h"

namespace Cjing3D::Win32
{
	class KeyBoardWin32 : public KeyBoard
	{
	public:
		void ProcessKeyboardEvent(const RAWKEYBOARD& keyboard);
	};
}