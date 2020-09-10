#include "keyboard.h"
#include "input\InputSystem.h"

#include <limits>
#include <type_traits>
#include <utility>

namespace Cjing3D
{
	void KeyBoard::SetKeyState(const KeyCode& key, const KeyState& state)
	{
		using SizeType = std::underlying_type<KeyCode>::type;
		mKeyStates[static_cast<SizeType>(key)] = (state == KeyState::KeyState_Down);
	}

	KeyState KeyBoard::GetKeyState(const KeyCode& key) const
	{
		using SizeType = std::underlying_type<KeyCode>::type;
		return mKeyStates[static_cast<SizeType>(key)] ? KeyState_Down : KeyState_Up;
	}

	bool KeyBoard::IsKeyDown(const KeyCode& key) const
	{
		using SizeType = std::underlying_type<KeyCode>::type;
		return mKeyStates[static_cast<SizeType>(key)];
	}

	bool KeyBoard::IsConnected(U32 index) const
	{
		return index == 0;
	}

	U32 KeyBoard::GetMaxGamepadController() const
	{
		return 1;
	}
}