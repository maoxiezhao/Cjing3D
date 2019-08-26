#include "InputSystem.h"
#include "input\directInput.h"

namespace Cjing3D
{

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