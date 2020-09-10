#include "keyboardWin32.h"

#include <optional>

namespace Cjing3D::Win32
{
	namespace {
		std::optional<KeyCode> ConvertToKeyCode(USHORT rawKeyCode)
		{
			switch (rawKeyCode)
			{
				case VK_F1:      return KeyCode::F1;
				case VK_F2:      return KeyCode::F2;
				case VK_F3:      return KeyCode::F3;
				case VK_F4:      return KeyCode::F4;
				case VK_F5:      return KeyCode::F5;
				case VK_F6:      return KeyCode::F6;
				case VK_F7:      return KeyCode::F7;
				case VK_F8:      return KeyCode::F8;
				case VK_F9:      return KeyCode::F9;
				case VK_F10:     return KeyCode::F10;
				case VK_F11:     return KeyCode::F11;
				case VK_F12:     return KeyCode::F12;
				case VK_F13:     return KeyCode::F13;
				case VK_F14:     return KeyCode::F14;
				case VK_F15:     return KeyCode::F15;
				case 0x41:       return KeyCode::A;
				case 0x42:       return KeyCode::B;
				case 0x43:       return KeyCode::C;
				case 0x44:       return KeyCode::D;
				case 0x45:       return KeyCode::E;
				case 0x46:       return KeyCode::F;
				case 0x47:       return KeyCode::G;
				case 0x48:       return KeyCode::H;
				case 0x49:       return KeyCode::I;
				case 0x4A:       return KeyCode::J;
				case 0x4B:       return KeyCode::K;
				case 0x4C:       return KeyCode::L;
				case 0x4D:       return KeyCode::M;
				case 0x4E:       return KeyCode::N;
				case 0x4F:       return KeyCode::O;
				case 0x50:       return KeyCode::P;
				case 0x51:       return KeyCode::Q;
				case 0x52:       return KeyCode::R;
				case 0x53:       return KeyCode::S;
				case 0x54:       return KeyCode::T;
				case 0x55:       return KeyCode::U;
				case 0x56:       return KeyCode::V;
				case 0x57:       return KeyCode::W;
				case 0x58:       return KeyCode::X;
				case 0x59:       return KeyCode::Y;
				case 0x5A:       return KeyCode::Z;
				case 0x30:       return KeyCode::Alpha0;
				case 0x31:       return KeyCode::Alpha1;
				case 0x32:       return KeyCode::Alpha2;
				case 0x33:       return KeyCode::Alpha3;
				case 0x34:       return KeyCode::Alpha4;
				case 0x35:       return KeyCode::Alpha5;
				case 0x36:       return KeyCode::Alpha6;
				case 0x37:       return KeyCode::Alpha7;
				case 0x38:       return KeyCode::Alpha8;
				case 0x39:       return KeyCode::Alpha9;
				case VK_NUMPAD0: return KeyCode::Keypad0;
				case VK_NUMPAD1: return KeyCode::Keypad1;
				case VK_NUMPAD2: return KeyCode::Keypad2;
				case VK_NUMPAD3: return KeyCode::Keypad3;
				case VK_NUMPAD4: return KeyCode::Keypad4;
				case VK_NUMPAD5: return KeyCode::Keypad5;
				case VK_NUMPAD6: return KeyCode::Keypad6;
				case VK_NUMPAD7: return KeyCode::Keypad7;
				case VK_NUMPAD8: return KeyCode::Keypad8;
				case VK_NUMPAD9: return KeyCode::Keypad9;
				case VK_UP:      return KeyCode::Arrow_Up;
				case VK_DOWN:    return KeyCode::Arrow_Down;
				case VK_LEFT:    return KeyCode::Arrow_Left;
				case VK_RIGHT:   return KeyCode::Arrow_Right;
				case VK_INSERT:  return KeyCode::Insert;
				case VK_HOME:    return KeyCode::Home;
				case VK_END:     return KeyCode::End;
				case VK_NEXT:    return KeyCode::Page_Down;
				case VK_PRIOR:   return KeyCode::Page_Up;
				case VK_BACK:    return KeyCode::Backspace;
				case VK_DELETE:  return KeyCode::Delete;
				case VK_TAB:     return KeyCode::Tab;
				case VK_RETURN:  return KeyCode::Enter;
				case VK_PAUSE:   return KeyCode::Pause;
				case VK_ESCAPE:  return KeyCode::Esc;
				case VK_SPACE:   return KeyCode::Space;
				case VK_SNAPSHOT:return KeyCode::PrintScreen;
				case VK_NUMLOCK: return KeyCode::NumLock;
				case VK_CAPITAL: return KeyCode::CapsLock;
				case VK_MENU:    return KeyCode::Alt_Left;
				case VK_CONTROL: return KeyCode::Ctrl_Left;
				case VK_LCONTROL:return KeyCode::Ctrl_Left;
				case VK_RCONTROL:return KeyCode::Ctrl_Right;
				case VK_SHIFT:   return KeyCode::Shift_Left;
				case VK_LSHIFT:  return KeyCode::Shift_Left;
				case VK_RSHIFT:  return KeyCode::Shift_Right;
				default:
					break;
			}
			return std::nullopt;
		}
	}

	void KeyBoardWin32::ProcessKeyboardEvent(const RAWKEYBOARD& keyboard)
	{
		const auto keyPtr = ConvertToKeyCode(keyboard.VKey);
		if (!keyPtr) {
			return;
		}

		KeyState keyState = (keyboard.Flags & RI_KEY_BREAK ? KeyState_Up : KeyState_Down);
		KeyCode keyCode = (*keyPtr);

		bool isKeyAlreadyDown = IsKeyDown(keyCode);
		SetKeyState(keyCode, keyState);
	}
}