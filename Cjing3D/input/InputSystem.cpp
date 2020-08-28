#include "InputSystem.h"

namespace Cjing3D
{
	InputManager::InputManager()
	{
	}

	InputManager::~InputManager()
	{
	}

	void InputManager::Update(F32 deltaTime)
	{
	}

	bool InputManager::IsKeyPressed(const KeyCode key) const
	{
		return GetKeyBoard()->IsKeyPressed(key);
	}

	bool InputManager::IsKeyReleased(const KeyCode key)const
	{
		return GetKeyBoard()->IsKeyReleased(key);
	}

	bool InputManager::IsKeyHold(const KeyCode key)const
	{
		return GetKeyBoard()->IsKeyHold(key);
	}

	I32x2 InputManager::GetMousePos() const
	{
		return I32x2();
	}

	F32 InputManager::GetMouseWheelDelta() const
	{
		return 0;
	}

	bool InputManager::IsGamepadConnected() const
	{
		return false;
	}

	bool InputManager::IsGamepadButtonDown() const
	{
		return false;
	}

	bool InputManager::IsGamepadButtonUp() const
	{
		return false;
	}

	bool InputManager::IsGamepadButtonHold() const
	{
		return false;
	}

	F32x2 InputManager::GetGamepadThumbStickLeft() const
	{
		return F32x2();
	}

	F32x2 InputManager::GetGamepadThumbStickRight() const
	{
		return F32x2();
	}
}