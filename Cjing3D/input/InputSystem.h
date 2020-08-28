#pragma once

#include "common\common.h"
#include "core\subSystem.hpp"
#include "input\keyboard.h"
#include "input\mouse.h"
#include "input\gamepad.h"

namespace Cjing3D
{
LUA_BINDER_REGISTER_CLASS
class InputManager : public SubSystem
{
public:
	InputManager();
	virtual ~InputManager();

	virtual void Update(F32 deltaTime) = 0;

	// keyboard
	LUA_BINDER_REGISTER_CLASS_METHOD_FUNCTION
	bool IsKeyPressed(const KeyCode key)const;
	LUA_BINDER_REGISTER_CLASS_METHOD_FUNCTION
	bool IsKeyReleased(const KeyCode key)const;
	LUA_BINDER_REGISTER_CLASS_METHOD_FUNCTION
	bool IsKeyHold(const KeyCode key)const;

	// mouse
	LUA_BINDER_REGISTER_CLASS_METHOD_FUNCTION
	I32x2 GetMousePos()const;
	LUA_BINDER_REGISTER_CLASS_METHOD_FUNCTION
	F32 GetMouseWheelDelta()const;

	// gamepad
	LUA_BINDER_REGISTER_CLASS_METHOD_FUNCTION
	bool IsGamepadConnected()const;
	LUA_BINDER_REGISTER_CLASS_METHOD_FUNCTION
	bool IsGamepadButtonDown()const;
	LUA_BINDER_REGISTER_CLASS_METHOD_FUNCTION
	bool IsGamepadButtonUp()const;
	LUA_BINDER_REGISTER_CLASS_METHOD_FUNCTION
	bool IsGamepadButtonHold()const;
	LUA_BINDER_REGISTER_CLASS_METHOD_FUNCTION
	F32x2 GetGamepadThumbStickLeft()const;
	LUA_BINDER_REGISTER_CLASS_METHOD_FUNCTION
	F32x2 GetGamepadThumbStickRight()const;

	virtual const std::shared_ptr<KeyBoard> GetKeyBoard()const = 0;
	virtual const std::shared_ptr<Mouse>    GetMouse()const = 0;
	virtual const std::shared_ptr<Gamepad>  GetGamePad()const = 0;
};
}