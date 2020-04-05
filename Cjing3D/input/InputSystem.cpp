#include "InputSystem.h"
#include "input\directInput.h"
#include "helper\profiler.h"

namespace Cjing3D
{

ENUM_TRAITS_REGISTER_ENUM_BEGIN(KeyCode)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(F1)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(F2)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(F3)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(F4)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(F5)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(F6)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(F7)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(F8)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(F9)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(F10)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(F11)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(F12)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(F13)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(F14)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(F15)
/*FUNCTIONS*/
ENUM_TRAITS_REGISTER_ENUM_DEFINE(Alpha0)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(Alpha1)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(Alpha2)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(Alpha3)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(Alpha4)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(Alpha5)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(Alpha6)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(Alpha7)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(Alpha8)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(Alpha9)
/*Numbers*/
ENUM_TRAITS_REGISTER_ENUM_DEFINE(Keypad0)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(Keypad1)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(Keypad2)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(Keypad3)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(Keypad4)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(Keypad5)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(Keypad6)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(Keypad7)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(Keypad8)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(Keypad9)
/*Numpad*/
ENUM_TRAITS_REGISTER_ENUM_DEFINE(Q)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(W)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(E)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(R)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(T)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(Y)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(U)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(I)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(O)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(P)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(A)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(S)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(D)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(F)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(G)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(H)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(J)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(K)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(L)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(Z)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(X)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(C)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(V)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(B)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(N)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(M)
/*Letters*/
ENUM_TRAITS_REGISTER_ENUM_DEFINE(Esc)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(Tab)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(Shift_Left)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(Shift_Right)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(Ctrl_Left)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(Ctrl_Right)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(Alt_Left)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(Alt_Right)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(Space)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(CapsLock)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(Backspace)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(Enter)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(Delete)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(Arrow_Left)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(Arrow_Right)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(Arrow_Up)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(Arrow_Down)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(Page_Up)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(Page_Down)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(Home)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(End)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(Insert)
// Mouse
ENUM_TRAITS_REGISTER_ENUM_DEFINE(Click_Left)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(Click_Middle)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(Click_Right)
ENUM_TRAITS_REGISTER_ENUM_END(KeyCode)

std::unique_ptr<DirectInput> mDirectInput = nullptr;

InputManager::InputManager(SystemContext & systemContext):
	SubSystem(systemContext)
{
}

InputManager::~InputManager()
{
}

void InputManager::Initialize(HWND windowHwnd, HINSTANCE windowInstance)
{
	mDirectInput = std::make_unique<DirectInput>();
	mDirectInput->Initialize(windowHwnd, windowInstance);
}

void InputManager::Uninitialize()
{
	mDirectInput->Uninitialize();
}

void InputManager::Update(F32 deltaTime)
{

	if (mDirectInput != nullptr) 
	{
		mDirectInput->Update();
	}
}

bool InputManager::IsKeyDown(const KeyCode key) const
{
	if (mDirectInput != nullptr) {
		return mDirectInput->IsKeyDown(key);
	}
	return false;
}

bool InputManager::IsKeyUp(const KeyCode key) const
{
	if (mDirectInput != nullptr) {
		return mDirectInput->IsKeyUp(key);
	}
	return false;
}

bool InputManager::IsKeyHold(const KeyCode key) const
{
	if (mDirectInput != nullptr) {
		return mDirectInput->IsKeyHold(key);
	}
	return false;
}

I32x2 InputManager::GetMousePos() const
{
	if (mDirectInput != nullptr) {
		return mDirectInput->GetMousePos();
	}
	return I32x2();
}

}