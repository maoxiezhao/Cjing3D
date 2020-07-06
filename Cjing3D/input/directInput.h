#pragma once

#include "common\common.h"
#include "input\InputSystem.h"

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <Xinput.h>
#pragma comment(lib,"dinput8.lib")


namespace Cjing3D {

	class DirectInput
	{
	private:
		IDirectInputDevice8 * mKeyBoard = nullptr;
		IDirectInputDevice8 * mMouse = nullptr;
		IDirectInput8*  mDirectInput = nullptr;

		DIMOUSESTATE mMouseState;
		I32x2 mMousePos = I32x2(0, 0);
		I32x2 mMouseDelta = I32x2(0, 0);
		I32 mMouseWheelDelta = 0;

		unsigned char   mKeyboardState[256];
		std::array<bool, 99> mCurrentKeyStatus;
		std::array<bool, 99> mPreviousKeyStatus;

		HWND mCurrentWindowHwne;

	public:
		DirectInput() {}

		void Initialize(HWND windowHwnd, HINSTANCE windowInstance);
		void Uninitialize();
		void Update();
		bool ReadKeyboardStates();
		bool ReadMouseState();
		bool IsKeyDown(const KeyCode key)const;
		bool IsKeyUp(const KeyCode key)const;
		bool IsKeyHold(const KeyCode key)const;

		I32x2 GetMousePos() const { return mMousePos; }
		I32 GetMouseWheelDelta()const { return mMouseWheelDelta; }
	};
}