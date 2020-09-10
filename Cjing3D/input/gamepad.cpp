#include "gamepad.h"

namespace Cjing3D
{
	U32 Gamepad::GetMaxGamepadController() const
	{
		return mGamepadControllers.size();
	}

	bool Gamepad::IsConnected(U32 index) const
	{
		if (index >= mGamepadControllers.size()) {
			return false;
		}

		return mGamepadControllers[index].mIsConnected;
	}

	bool Gamepad::IsKeyDown(const KeyCode& key, int index) const
	{
		if (key > Gamepad_CountStart && key < Gamepad_CountEnd)
		{
			if (!IsConnected(index)) {
				return false;
			}

			U32 buttonIndex = key - Gamepad_CountStart - 1;
			return mGamepadControllers[index].mState.mButtons[buttonIndex];
		}
		else
		{
			return false;
		}
	}
}