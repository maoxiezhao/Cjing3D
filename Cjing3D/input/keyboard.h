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

		virtual void Initialize();
		virtual void Update();
		virtual void Uninitialize();

		bool IsKeyPressed(const KeyCode& key)const;
		bool IsKeyReleased(const KeyCode& key)const;
		bool IsKeyHold(const KeyCode& key, U32 frames = 16, bool continuous = true)const;

		void SetKeyState(const KeyCode& key, const KeyState& state);
		bool IsKeyDown(const KeyCode& key)const;

	private:
		std::bitset<128> mKeyStates;
		std::map<KeyCode, U32> mInputKeys;
	};
}
