#include "inputSystemWin32.h"

namespace Cjing3D::Win32
{
	InputManagerWin32::InputManagerWin32()
	{
	}

	InputManagerWin32::~InputManagerWin32()
	{
	}

	void InputManagerWin32::Initialize(GameWindowWin32& gameWindow)
	{
		mKeyBoard = std::make_shared<KeyBoardWin32>();
		mKeyBoard->Initialize();
	}

	void InputManagerWin32::Update(F32 deltaTime)
	{
		if (mKeyBoard != nullptr) {
			mKeyBoard->Update();
		}
	}

	void InputManagerWin32::Uninitialize()
	{
		mKeyBoard->Uninitialize();
		mKeyBoard = nullptr;

		InputManager::Uninitialize();
	}

	void InputManagerWin32::ProcessMouseEvent(const RAWMOUSE& mouse)
	{
	}

	void InputManagerWin32::ProcessKeyboardEvent(const RAWKEYBOARD& keyboard)
	{
		mKeyBoard->ProcessKeyboardEvent(keyboard);
	}

	const std::shared_ptr<KeyBoard> InputManagerWin32::GetKeyBoard() const
	{
		return mKeyBoard;
	}

	const std::shared_ptr<Mouse> InputManagerWin32::GetMouse() const
	{
		return std::shared_ptr<Mouse>();
	}

	const std::shared_ptr<Gamepad> InputManagerWin32::GetGamePad() const
	{
		return std::shared_ptr<Gamepad>();
	}
}