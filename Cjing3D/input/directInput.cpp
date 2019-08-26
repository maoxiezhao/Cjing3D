#include "directInput.h"

namespace Cjing3D
{
	void DirectInput::Initialize(HWND windowHwnd, HINSTANCE windowInstance)
	{
		mCurrentWindowHwne = windowHwnd;

		SetForegroundWindow(windowHwnd);
		SetFocus(windowHwnd);

		auto result = DirectInput8Create(windowInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&mDirectInput, NULL);
		Debug::ThrowIfFailed(result, "Failed to create direct8Input:%08x", result);

		{
			result = mDirectInput->CreateDevice(GUID_SysKeyboard, &mKeyBoard, nullptr);
			Debug::ThrowIfFailed(result, "Failed to create keyboard device:%08x", result);

			result = mKeyBoard->SetDataFormat(&c_dfDIKeyboard);
			Debug::ThrowIfFailed(result, "Failed to set keyboard data format:%08x", result);

			result = mKeyBoard->SetCooperativeLevel(windowHwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
			Debug::ThrowIfFailed(result, "Failed to set cooperative level:%08x", result);

			result = mKeyBoard->Acquire();
			Debug::ThrowIfFailed(result, "Failed to acquire keyboard:%08x", result);
		}
		{
			result = mDirectInput->CreateDevice(GUID_SysMouse, &mMouse, nullptr);
			Debug::ThrowIfFailed(result, "Failed to create mouse device:%08x", result);

			result = mMouse->SetDataFormat(&c_dfDIMouse);
			Debug::ThrowIfFailed(result, "Failed to set mouse data format:%08x", result);

			result = mMouse->SetCooperativeLevel(windowHwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
			Debug::ThrowIfFailed(result, "Failed to set cooperative level:%08x", result);

			result = mMouse->Acquire();
			Debug::ThrowIfFailed(result, "Failed to acquire keyboard:%08x", result);
		}
	}

	void DirectInput::Uninitialize()
	{
		if (mKeyBoard != nullptr)
		{
			mKeyBoard->Unacquire();
			mKeyBoard->Release();
			mKeyBoard = nullptr;
		}

		if (mMouse != nullptr)
		{
			mMouse->Unacquire();
			mMouse->Release();
			mMouse = nullptr;
		}

		if (mDirectInput != nullptr)
		{
			mDirectInput->Release();
			mDirectInput = nullptr;
		}
	}

	void DirectInput::Update()
	{
		mPreviousKeyStatus = mCurrentKeyStatus;

		if (ReadKeyboardStates() == true)
		{
			// FUNCTION
			mCurrentKeyStatus[0] = mKeyboardState[DIK_F1] & 0x80;
			mCurrentKeyStatus[1] = mKeyboardState[DIK_F2] & 0x80;
			mCurrentKeyStatus[2] = mKeyboardState[DIK_F3] & 0x80;
			mCurrentKeyStatus[3] = mKeyboardState[DIK_F4] & 0x80;
			mCurrentKeyStatus[4] = mKeyboardState[DIK_F5] & 0x80;
			mCurrentKeyStatus[5] = mKeyboardState[DIK_F6] & 0x80;
			mCurrentKeyStatus[6] = mKeyboardState[DIK_F7] & 0x80;
			mCurrentKeyStatus[7] = mKeyboardState[DIK_F8] & 0x80;
			mCurrentKeyStatus[8] = mKeyboardState[DIK_F9] & 0x80;
			mCurrentKeyStatus[9] = mKeyboardState[DIK_F10] & 0x80;
			mCurrentKeyStatus[10] = mKeyboardState[DIK_F11] & 0x80;
			mCurrentKeyStatus[11] = mKeyboardState[DIK_F12] & 0x80;
			mCurrentKeyStatus[12] = mKeyboardState[DIK_F13] & 0x80;
			mCurrentKeyStatus[13] = mKeyboardState[DIK_F14] & 0x80;
			mCurrentKeyStatus[14] = mKeyboardState[DIK_F15] & 0x80;
			// NUMBERS
			mCurrentKeyStatus[15] = mKeyboardState[DIK_0] & 0x80;
			mCurrentKeyStatus[16] = mKeyboardState[DIK_1] & 0x80;
			mCurrentKeyStatus[17] = mKeyboardState[DIK_2] & 0x80;
			mCurrentKeyStatus[18] = mKeyboardState[DIK_3] & 0x80;
			mCurrentKeyStatus[19] = mKeyboardState[DIK_4] & 0x80;
			mCurrentKeyStatus[20] = mKeyboardState[DIK_5] & 0x80;
			mCurrentKeyStatus[21] = mKeyboardState[DIK_6] & 0x80;
			mCurrentKeyStatus[22] = mKeyboardState[DIK_7] & 0x80;
			mCurrentKeyStatus[23] = mKeyboardState[DIK_8] & 0x80;
			mCurrentKeyStatus[24] = mKeyboardState[DIK_9] & 0x80;
			// KEYPAD
			mCurrentKeyStatus[25] = mKeyboardState[DIK_NUMPAD0] & 0x80;
			mCurrentKeyStatus[26] = mKeyboardState[DIK_NUMPAD1] & 0x80;
			mCurrentKeyStatus[27] = mKeyboardState[DIK_NUMPAD2] & 0x80;
			mCurrentKeyStatus[28] = mKeyboardState[DIK_NUMPAD3] & 0x80;
			mCurrentKeyStatus[29] = mKeyboardState[DIK_NUMPAD4] & 0x80;
			mCurrentKeyStatus[30] = mKeyboardState[DIK_NUMPAD5] & 0x80;
			mCurrentKeyStatus[31] = mKeyboardState[DIK_NUMPAD6] & 0x80;
			mCurrentKeyStatus[32] = mKeyboardState[DIK_NUMPAD7] & 0x80;
			mCurrentKeyStatus[33] = mKeyboardState[DIK_NUMPAD8] & 0x80;
			mCurrentKeyStatus[34] = mKeyboardState[DIK_NUMPAD9] & 0x80;
			// LETTERS
			mCurrentKeyStatus[35] = mKeyboardState[DIK_Q] & 0x80;
			mCurrentKeyStatus[36] = mKeyboardState[DIK_W] & 0x80;
			mCurrentKeyStatus[37] = mKeyboardState[DIK_E] & 0x80;
			mCurrentKeyStatus[38] = mKeyboardState[DIK_R] & 0x80;
			mCurrentKeyStatus[39] = mKeyboardState[DIK_T] & 0x80;
			mCurrentKeyStatus[40] = mKeyboardState[DIK_Y] & 0x80;
			mCurrentKeyStatus[41] = mKeyboardState[DIK_U] & 0x80;
			mCurrentKeyStatus[42] = mKeyboardState[DIK_I] & 0x80;
			mCurrentKeyStatus[43] = mKeyboardState[DIK_O] & 0x80;
			mCurrentKeyStatus[44] = mKeyboardState[DIK_P] & 0x80;
			mCurrentKeyStatus[45] = mKeyboardState[DIK_A] & 0x80;
			mCurrentKeyStatus[46] = mKeyboardState[DIK_S] & 0x80;
			mCurrentKeyStatus[47] = mKeyboardState[DIK_D] & 0x80;
			mCurrentKeyStatus[48] = mKeyboardState[DIK_F] & 0x80;
			mCurrentKeyStatus[49] = mKeyboardState[DIK_G] & 0x80;
			mCurrentKeyStatus[50] = mKeyboardState[DIK_H] & 0x80;
			mCurrentKeyStatus[51] = mKeyboardState[DIK_J] & 0x80;
			mCurrentKeyStatus[52] = mKeyboardState[DIK_K] & 0x80;
			mCurrentKeyStatus[53] = mKeyboardState[DIK_L] & 0x80;
			mCurrentKeyStatus[54] = mKeyboardState[DIK_Z] & 0x80;
			mCurrentKeyStatus[55] = mKeyboardState[DIK_X] & 0x80;
			mCurrentKeyStatus[56] = mKeyboardState[DIK_C] & 0x80;
			mCurrentKeyStatus[57] = mKeyboardState[DIK_V] & 0x80;
			mCurrentKeyStatus[58] = mKeyboardState[DIK_B] & 0x80;
			mCurrentKeyStatus[59] = mKeyboardState[DIK_N] & 0x80;
			mCurrentKeyStatus[60] = mKeyboardState[DIK_M] & 0x80;
			// CONTROLS
			mCurrentKeyStatus[61] = mKeyboardState[DIK_ESCAPE] & 0x80;
			mCurrentKeyStatus[62] = mKeyboardState[DIK_TAB] & 0x80;
			mCurrentKeyStatus[63] = mKeyboardState[DIK_LSHIFT] & 0x80;
			mCurrentKeyStatus[64] = mKeyboardState[DIK_RSHIFT] & 0x80;
			mCurrentKeyStatus[65] = mKeyboardState[DIK_LCONTROL] & 0x80;
			mCurrentKeyStatus[66] = mKeyboardState[DIK_RCONTROL] & 0x80;
			mCurrentKeyStatus[67] = mKeyboardState[DIK_LALT] & 0x80;
			mCurrentKeyStatus[68] = mKeyboardState[DIK_RALT] & 0x80;
			mCurrentKeyStatus[69] = mKeyboardState[DIK_SPACE] & 0x80;
			mCurrentKeyStatus[70] = mKeyboardState[DIK_CAPSLOCK] & 0x80;
			mCurrentKeyStatus[71] = mKeyboardState[DIK_BACKSPACE] & 0x80;
			mCurrentKeyStatus[72] = mKeyboardState[DIK_RETURN] & 0x80;
			mCurrentKeyStatus[73] = mKeyboardState[DIK_DELETE] & 0x80;
			mCurrentKeyStatus[74] = mKeyboardState[DIK_LEFTARROW] & 0x80;
			mCurrentKeyStatus[75] = mKeyboardState[DIK_RIGHTARROW] & 0x80;
			mCurrentKeyStatus[76] = mKeyboardState[DIK_UPARROW] & 0x80;
			mCurrentKeyStatus[77] = mKeyboardState[DIK_DOWNARROW] & 0x80;
			mCurrentKeyStatus[78] = mKeyboardState[DIK_PGUP] & 0x80;
			mCurrentKeyStatus[79] = mKeyboardState[DIK_PGDN] & 0x80;
			mCurrentKeyStatus[80] = mKeyboardState[DIK_HOME] & 0x80;
			mCurrentKeyStatus[81] = mKeyboardState[DIK_END] & 0x80;
			mCurrentKeyStatus[82] = mKeyboardState[DIK_INSERT] & 0x80;
		}
		else
		{
			for (U32 i = 0; i < 99; i++) {
				mCurrentKeyStatus[i] = false;
			}
		}

		if (ReadMouseState() == true)
		{
			int mouseStartIndex = Click_Left;
			mCurrentKeyStatus[mouseStartIndex] = mMouseState.rgbButtons[0] & 0x80;
			mCurrentKeyStatus[mouseStartIndex + 1] = mMouseState.rgbButtons[3] & 0x80;
			mCurrentKeyStatus[mouseStartIndex + 2] = mMouseState.rgbButtons[1] & 0x80;

			mMouseDelta[0] = static_cast<F32>(mMouseState.lX);
			mMouseDelta[1] = static_cast<F32>(mMouseState.lY);
			mMouseWheelDelta = static_cast<F32>(mMouseState.lZ);

			POINT mouseScreenPos;
			if (GetCursorPos(&mouseScreenPos))
			{
				HWND focusedHwnd = GetActiveWindow();
				if (focusedHwnd == mCurrentWindowHwne)
				{
					POINT mouseClientPos = mouseScreenPos;
					ScreenToClient(focusedHwnd, &mouseClientPos);

					mMousePos[0] = mouseClientPos.x;
					mMousePos[1] = mouseClientPos.y;
				}
			}
		}
		else
		{
			int mouseStartIndex = Click_Left;
			mCurrentKeyStatus[mouseStartIndex] = false;
			mCurrentKeyStatus[mouseStartIndex + 1] = false;
			mCurrentKeyStatus[mouseStartIndex + 2] = false;

			mMouseDelta[0] = 0.0f;
			mMouseDelta[1] = 0.0f;
			mMouseWheelDelta = 0.0f;
			mMousePos[0] = 0;
			mMousePos[1] = 0;
		}
	}

	bool DirectInput::ReadKeyboardStates()
	{
		auto result = mKeyBoard->GetDeviceState(sizeof(mKeyboardState), (LPVOID)(&mKeyboardState));
		if (SUCCEEDED(result))
		{
			return true;
		}
		else if (result == DIERR_INPUTLOST || result == DIERR_NOTACQUIRED)
		{
			mKeyBoard->Acquire();
		}
		return false;
	}

	bool DirectInput::ReadMouseState()
	{
		auto result = mMouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)(&mMouseState));
		if (SUCCEEDED(result))
		{
			return true;
		}
		else if (result == DIERR_INPUTLOST || result == DIERR_NOTACQUIRED)
		{
			mMouse->Acquire();
		}
		return false;
	}

	bool DirectInput::IsKeyDown(const KeyCode key)const
	{
		return mCurrentKeyStatus[key] && !mPreviousKeyStatus[key];
	}

	bool DirectInput::IsKeyUp(const KeyCode key)const
	{
		return !mCurrentKeyStatus[key] && mPreviousKeyStatus[key];
	}

	bool DirectInput::IsKeyHold(const KeyCode key)const
	{
		return mCurrentKeyStatus[key];
	}

	I32x2 DirectInput::GetMousePos() const
	{
		return mMousePos;
	}
}