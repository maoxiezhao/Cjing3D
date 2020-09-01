#pragma once

#include "common\common.h"
#include "keyCode.h"

namespace Cjing3D
{
	struct MouseState
	{
		I32x2 mMousePos = I32x2(0, 0);
		I32x2 mMouseDelta = I32x2(0, 0);
		F32 mMouseWheelDelta = 0.0f;
		KeyState mLeftButton = KeyState_Up;
		KeyState mMiddleButton = KeyState_Up;
		KeyState mRightButton = KeyState_Up;
	};

	class Mouse
	{
	public:
		Mouse() = default;

		I32x2 GetMousePos()const { return mMouseState.mMousePos; }
		I32x2 GetMouseDelta()const { return mMouseState.mMouseDelta; }
		F32 GetMouseWheelDelta()const { return mMouseState.mMouseWheelDelta; }
		MouseState GetMouseState()const { return mMouseState; }
		MouseState GetPrevMouseState()const { return mPrevMouseState; }

		bool IsKeyDown(const KeyCode& key)const
		{
			KeyState keyState = KeyState_Up;
			switch (key)
			{
			case Click_Left:
				keyState = mMouseState.mLeftButton;
			case Click_Middle:
				keyState = mMouseState.mMiddleButton;
			case Click_Right:
				keyState = mMouseState.mRightButton;
			default:
				break;
			}
			return keyState == KeyState_Down;
		}

	protected:
		MouseState mMouseState;
		MouseState mPrevMouseState;
	};
}
