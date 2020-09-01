#include "InputSystem.h"

namespace Cjing3D
{
	InputManager::InputManager()
	{
	}

	InputManager::~InputManager()
	{
	}

	void InputManager::Initialize()
	{
	}

	void InputManager::Update(F32 deltaTime)
	{
		// update gamepad
		auto gamepad = GetGamePad();
		if (gamepad != nullptr)
		{
			gamepad->Update();
		
			for (int i = 0; i < gamepad->GetMaxGamepadController(); i++)
			{
				if (gamepad->IsConnected(i)) {
					RegisterController(i, InputController::GAMEPAD);
				}
			}
		}

		// update key board
		auto keyboard = GetKeyBoard();
		if (keyboard != nullptr)
		{
			for (int i = 0; i < keyboard->GetMaxGamepadController(); i++)
			{
				if (keyboard->IsConnected(i)) {
					RegisterController(i, InputController::RAWINPUT);
				}
			}
		}

		// update controller
		for (auto& controller : mInputControllers)
		{
			bool connected = false;
			switch (controller.mType)
			{
			case InputController::GAMEPAD:
				connected = gamepad != nullptr ? gamepad->IsConnected(controller.mIndex) : false;
				break;
			case InputController::RAWINPUT:
				connected = keyboard != nullptr ? keyboard->IsConnected(controller.mIndex) : false;
				break;
			}

			if (!connected) {
				controller.mType = InputController::UNKNOWN;
			}
		}

		// update input keys
		for (auto it = mInputKeys.begin(); it != mInputKeys.end();)
		{
			KeyCode keyCode = it->first.mKeyCode;
			U32 index = it->first.mControllerIndex;
			U32 inputSecond = it->second;

			bool toRemoved = false;
			if (IsKeyDown(keyCode, index)) {
				it->second = inputSecond + 1;
			}
			else {
				toRemoved = true;
			}

			if (toRemoved) {
				mInputKeys.erase(it++);
			}
			else {
				++it;
			}
		}
	}

	void InputManager::Uninitialize()
	{
		mInputKeys.clear();
	}

	bool InputManager::IsKeyPressed(const KeyCode& key, U32 index)
	{
		if (!IsKeyDown(key, index)) {
			return false;
		}

		// 如果按键从未按下或者当前帧按下，则视为press
		InputInst inst = {};
		inst.mKeyCode = key;
		inst.mControllerIndex = index;

		auto it = mInputKeys.find(inst);
		if (it == mInputKeys.end())
		{
			mInputKeys[inst] = 0;
			return true;
		}
		else if (it->second == 0) {
			return true;
		}

		return false;
	}

	bool InputManager::IsKeyReleased(const KeyCode& key, U32 index)
	{
		if (IsKeyDown(key, index)) {
			return false;
		}

		InputInst inst = {};
		inst.mKeyCode = key;
		inst.mControllerIndex = index;

		auto it = mInputKeys.find(inst);
		if (it != mInputKeys.end() && it->second > 0) {
			return true;
		}
		else
		{
			return false;
		}
	}

	bool InputManager::IsKeyHold(const KeyCode& key, U32 index, U32 frames, bool continuous)
	{
		if (!IsKeyDown(key, index)) {
			return false;
		}

		InputInst inst = {};
		inst.mKeyCode = key;
		inst.mControllerIndex = index;

		auto it = mInputKeys.find(inst);
		if (it == mInputKeys.end())
		{
			mInputKeys[inst] = 0;
			return false;
		}
		else if ((!continuous && it->second == frames) ||
				  (continuous && it->second >= frames))
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	bool InputManager::IsKeyDown(const KeyCode& key, U32 index) const
	{
		if (index == 0 && key < KeyCode::Gamepad_CountStart)
		{
			switch (key)
			{
			case Click_Left:
			case Click_Middle:
			case Click_Right:
				return GetMouse() != nullptr ? GetMouse()->IsKeyDown(key) : false;
				break;
			default:
				return GetKeyBoard() != nullptr ? GetKeyBoard()->IsKeyDown(key) : false;
				break;
			}
			return false;
		}
		else
		{
			auto gamepad = GetGamePad();
			if (gamepad == nullptr) {
				return false;
			}

			return gamepad->IsKeyDown(key, index);
		}
	}

	I32x2 InputManager::GetMousePos() const
	{
		return GetMouse()->GetMousePos();
	}

	F32 InputManager::GetMouseWheelDelta() const
	{
		return GetMouse()->GetMouseWheelDelta();
	}

	MouseState InputManager::GetMouseState() const
	{
		return GetMouse()->GetMouseState();
	}

	MouseState InputManager::GetPeveMouseState() const
	{
		return GetMouse()->GetPrevMouseState();
	}

	void InputManager::RegisterController(U32 index, InputController::ControllerType type)
	{
		int slot = -1;
		for (int i = 0; i < (int)mInputControllers.size(); ++i)
		{
			auto& controller = mInputControllers[i];
			if (slot < 0 && controller.mType == InputController::UNKNOWN) {
				slot = i;
			}

			if (controller.mType == type && controller.mIndex == index)
			{
				slot = i;
				break;
			}
		}

		if (slot == -1)
		{
			slot = (int)mInputControllers.size();
			mInputControllers.emplace_back();
		}

		mInputControllers[slot].mType = type;
		mInputControllers[slot].mIndex = index;
	}
}