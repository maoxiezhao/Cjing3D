#pragma once

#if __has_include("xinput.h")

#include "common\common.h"
#include "platform\win32\includeWin32.h"
#include "input\gamepad.h"

#include <xinput.h>
#pragma comment(lib,"xinput.lib")

#include <array>

namespace Cjing3D::XInput
{


	class GamepadXInput : public Gamepad
	{
	public:
		GamepadXInput();
		~GamepadXInput();

		virtual void Update();
		virtual void SetFeedback(const GamepadFeedback& data, U32 index);

	private:
		XINPUT_STATE mInputStates[4] = {};
	};
}

#endif