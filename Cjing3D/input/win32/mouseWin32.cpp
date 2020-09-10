#include "mouseWin32.h"

namespace Cjing3D::Win32
{
	MouseWin32::MouseWin32(HWND hWnd) :
		mHwnd(hWnd)
	{
	}

	void MouseWin32::ProcessMouseEvent(const RAWMOUSE& mouse)
	{
		// mouse move
		if (mouse.usFlags == MOUSE_MOVE_RELATIVE ||
			mouse.usFlags & MOUSE_MOVE_ABSOLUTE)
		{
			POINT cursorPos;
			GetCursorPos(&cursorPos);
			ScreenToClient(mHwnd, &cursorPos);

			mPrevMouseState = mMouseState;
			mMouseState.mMousePos = I32x2(cursorPos.x, cursorPos.y);
		}

		// mouse wheel
		if (mouse.usButtonFlags & RI_MOUSE_WHEEL) 
		{		
			mPrevMouseState = mMouseState;
			mMouseState.mMouseWheelDelta = *(SHORT*)(&mouse.usButtonData);
		}

		// mouse button
		if (mouse.usButtonFlags & RI_MOUSE_BUTTON_1_DOWN) 
		{
			mPrevMouseState = mMouseState;
			mMouseState.mLeftButton = KeyState_Down;
		}
		else if (mouse.usButtonFlags & RI_MOUSE_BUTTON_1_UP) 
		{
			mPrevMouseState = mMouseState;
			mMouseState.mLeftButton = KeyState_Up;
		}

		if (mouse.usButtonFlags & RI_MOUSE_BUTTON_2_DOWN) 
		{
			mPrevMouseState = mMouseState;
			mMouseState.mRightButton = KeyState_Down;
		}
		else if (mouse.usButtonFlags & RI_MOUSE_BUTTON_2_UP) 
		{
			mPrevMouseState = mMouseState;
			mMouseState.mRightButton = KeyState_Up;
		}

		if (mouse.usButtonFlags & RI_MOUSE_BUTTON_3_DOWN) 
		{
			mPrevMouseState = mMouseState;
			mMouseState.mMiddleButton = KeyState_Down;
		}
		else if (mouse.usButtonFlags & RI_MOUSE_BUTTON_3_UP) 
		{
			mPrevMouseState = mMouseState;
			mMouseState.mMiddleButton = KeyState_Up;
		}
	}
}
