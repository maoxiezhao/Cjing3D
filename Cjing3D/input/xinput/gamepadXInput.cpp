#include "gamepadXInput.h"

namespace Cjing3D::XInput
{
	namespace {
		float Deadzone(float x)
		{
			if ((x) > -0.24f && (x) < 0.24f)
				x = 0;
			if (x < -1.0f)
				x = -1.0f;
			if (x > 1.0f)
				x = 1.0f;
			return x;
		}
	}

	GamepadXInput::GamepadXInput()
	{
		for (int i = 0; i < mGamepadControllers.size(); i++)
		{
			mGamepadControllers[i].mIsConnected = false;
		}
	}

	GamepadXInput::~GamepadXInput()
	{
	}

	void GamepadXInput::Update()
	{
		for (int i = 0; i < mGamepadControllers.size(); i++)
		{
			auto& controller = mGamepadControllers[i];
			auto& xinputState = mInputStates[i];

			auto result = XInputGetState(i, &xinputState);
			if (result != ERROR_SUCCESS)
			{
				controller.mIsConnected = false;
			}
			else
			{
				controller.mIsConnected = true;

	
				auto& state = controller.mState;
				state.mLeftThumbStick = F32x2(
					Deadzone((F32)xinputState.Gamepad.sThumbLX / 32767.0f),
					Deadzone((F32)xinputState.Gamepad.sThumbLY / 32767.0f)
				);
				state.mRightThumbStick = F32x2(
					Deadzone((F32)xinputState.Gamepad.sThumbRX / 32767.0f),
					Deadzone((F32)xinputState.Gamepad.sThumbRY / 32767.0f)
				);
				state.mLeftTrigger = (F32)xinputState.Gamepad.bLeftTrigger / 255.0f;
				state.mLeftTrigger = (F32)xinputState.Gamepad.bRightTrigger / 255.0f;

				state.mButtons[0]  = (bool)(xinputState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP);
				state.mButtons[1]  = (bool)(xinputState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN);
				state.mButtons[2]  = (bool)(xinputState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT);
				state.mButtons[3]  = (bool)(xinputState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT);
				state.mButtons[4]  = (bool)(xinputState.Gamepad.wButtons & XINPUT_GAMEPAD_A);
				state.mButtons[5]  = (bool)(xinputState.Gamepad.wButtons & XINPUT_GAMEPAD_B);
				state.mButtons[6]  = (bool)(xinputState.Gamepad.wButtons & XINPUT_GAMEPAD_X);
				state.mButtons[7]  = (bool)(xinputState.Gamepad.wButtons & XINPUT_GAMEPAD_Y);
				state.mButtons[8]  = (bool)(xinputState.Gamepad.wButtons & XINPUT_GAMEPAD_START);
				state.mButtons[9]  = (bool)(xinputState.Gamepad.wButtons & XINPUT_GAMEPAD_BACK);
				state.mButtons[10] = (bool)(xinputState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER);
				state.mButtons[11] = (bool)(xinputState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER);
				state.mButtons[12] = (bool)(xinputState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB);
				state.mButtons[13] = (bool)(xinputState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB);
			}
		}
	}

	void GamepadXInput::SetFeedback(const GamepadFeedback& data, U32 index)
	{
		if (index < mGamepadControllers.size())
		{
			XINPUT_VIBRATION vibration = {};
			vibration.wLeftMotorSpeed = (WORD)(std::max(0.0f, std::min(1.0f, data.leftVibration)) * 65535);
			vibration.wRightMotorSpeed = (WORD)(std::max(0.0f, std::min(1.0f, data.rightVibration)) * 65535);
		
			XInputSetState((DWORD)index, &vibration);
		}
	}
}