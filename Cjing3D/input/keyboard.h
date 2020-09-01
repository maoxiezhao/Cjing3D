#pragma once

#include "common\common.h"
#include "input\keyCode.h"

#include <bitset>

namespace Cjing3D
{
	class KeyBoard
	{
	public:
		KeyBoard() = default;

		void SetKeyState(const KeyCode& key, const KeyState& state);
		KeyState GetKeyState(const KeyCode& key)const;
		bool IsKeyDown(const KeyCode& key)const;
		bool IsConnected(U32 index)const;
		U32 GetMaxGamepadController()const;

	private:
		std::bitset<128> mKeyStates;
	};
}
