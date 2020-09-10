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

	virtual void Initialize();
	virtual void Update(F32 deltaTime);
	virtual void Uninitialize();

	// keyboard
	LUA_BINDER_REGISTER_CLASS_METHOD_FUNCTION
	bool IsKeyDown(const KeyCode& key, U32 index = 0)const;
	LUA_BINDER_REGISTER_CLASS_METHOD_FUNCTION
	bool IsKeyPressed(const KeyCode& key, U32 index = 0);
	LUA_BINDER_REGISTER_CLASS_METHOD_FUNCTION
	bool IsKeyReleased(const KeyCode& key, U32 index = 0);
	LUA_BINDER_REGISTER_CLASS_METHOD_FUNCTION
	bool IsKeyHold(const KeyCode& key, U32 index = 0, U32 frames = 16, bool continuous = true);

	// mouse
	LUA_BINDER_REGISTER_CLASS_METHOD_FUNCTION
	I32x2 GetMousePos()const;
	LUA_BINDER_REGISTER_CLASS_METHOD_FUNCTION
	F32 GetMouseWheelDelta()const;
	MouseState GetMouseState()const;
	MouseState GetPeveMouseState()const;

	virtual const std::shared_ptr<KeyBoard> GetKeyBoard()const = 0;
	virtual const std::shared_ptr<Mouse>    GetMouse()const = 0;
	virtual const std::shared_ptr<Gamepad>  GetGamePad()const = 0;

private:
	struct InputInst
	{
		KeyCode mKeyCode = KeyCode::Unknown;
		U32 mControllerIndex = 0;

		bool operator<(const InputInst& rhs) {
			return (mKeyCode != rhs.mKeyCode || mControllerIndex != rhs.mControllerIndex);
		}

		struct LessComparer {
			bool operator()(const InputInst& a, const InputInst& b) const {
				return (a.mKeyCode < b.mKeyCode || a.mControllerIndex < b.mControllerIndex);
			}
		};
	};
	std::map<InputInst, U32, InputInst::LessComparer> mInputKeys;

	struct InputController
	{
		enum ControllerType
		{
			UNKNOWN,
			GAMEPAD,
			RAWINPUT,
		};
		ControllerType mType = UNKNOWN;
		int mIndex = 0;
	};
	std::vector<InputController> mInputControllers;

	void RegisterController(U32 index, InputController::ControllerType type);
};
}