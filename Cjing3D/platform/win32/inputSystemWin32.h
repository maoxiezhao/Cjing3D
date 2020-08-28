#pragma once

#include "platform\win32\gameWindowWin32.h"
#include "input\InputSystem.h"
#include "input\win32\keyboardWin32.h"

namespace Cjing3D::Win32
{
	class InputManagerWin32 : public InputManager
	{
	public:
		InputManagerWin32();
		virtual ~InputManagerWin32();

		void Initialize(GameWindowWin32& gameWindow);
		void Update(F32 deltaTime) override;
		void Uninitialize()override;
		void ProcessMouseEvent(const RAWMOUSE& mouse);
		void ProcessKeyboardEvent(const RAWKEYBOARD& keyboard);

		const std::shared_ptr<KeyBoard> GetKeyBoard()const override;
		const std::shared_ptr<Mouse>    GetMouse()const override;
		const std::shared_ptr<Gamepad>  GetGamePad()const override;

	private:
		std::shared_ptr<KeyBoardWin32> mKeyBoard = nullptr;
	};
}