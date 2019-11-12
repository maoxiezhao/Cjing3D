#pragma once

#include "common\common.h"
#include "core\subSystem.hpp"
#include "helper\enumInfo.h"

namespace Cjing3D
{

enum KeyCode
{
	// Keyboard
	F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, F13, F14, F15,/*Function*/
	Alpha0, Alpha1, Alpha2, Alpha3, Alpha4, Alpha5, Alpha6, Alpha7, Alpha8, Alpha9,/*Numbers*/
	Keypad0, Keypad1, Keypad2, Keypad3, Keypad4, Keypad5, Keypad6, Keypad7, Keypad8, Keypad9,/*Numpad*/
	Q, W, E, R, T, Y, U, I, O, P, A, S, D, F, G, H, J, K, L, Z, X, C, V, B, N, M, /*Letters*/
	Esc,
	Tab,
	Shift_Left, 
	Shift_Right,
	Ctrl_Left, 
	Ctrl_Right,
	Alt_Left, 
	Alt_Right,
	Space,
	CapsLock,
	Backspace,
	Enter,
	Delete,
	Arrow_Left, 
	Arrow_Right, 
	Arrow_Up,
	Arrow_Down,
	Page_Up, Page_Down,
	Home,
	End,
	Insert,
	// Mouse
	Click_Left,
	Click_Middle,
	Click_Right,
	key_count
};

LUA_BINDER_REGISTER_CLASS
class InputManager : public SubSystem
{
public:
	LUA_BINDER_REGISTER_CLASS_CONSTRUCTOR
	InputManager(SystemContext& systemContext);
	~InputManager();

	void Initialize(HWND windowHwnd, HINSTANCE windowInstance);
	void Uninitialize();
	virtual void Update(F32 deltaTime);

    LUA_BINDER_REGISTER_CLASS_METHOD_FUNCTION
	bool IsKeyDown(const KeyCode key)const;

	LUA_BINDER_REGISTER_CLASS_METHOD_FUNCTION
	bool IsKeyUp(const KeyCode key)const;

	LUA_BINDER_REGISTER_CLASS_METHOD_FUNCTION
	bool IsKeyHold(const KeyCode key)const;

	I32x2 GetMousePos()const;
};

ENUM_TRAITS_REGISTER_ENUM_HEADER(KeyCode)

}