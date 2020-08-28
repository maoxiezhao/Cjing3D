#include "keyboard.h"

#include <limits>
#include <type_traits>
#include <utility>

namespace Cjing3D
{
	bool KeyBoard::IsKeyPressed(const KeyCode& key)const
	{
		if (!IsKeyDown(key)) {
			return false;
		}

		// 如果按键从未按下或者当前帧按下，则视为press
		auto it = mInputKeys.find(key);
		return (it == mInputKeys.end() || it->second == 0);
	}

	bool KeyBoard::IsKeyReleased(const KeyCode& key)const
	{
		if (IsKeyDown(key)) {
			return false;
		}

		auto it = mInputKeys.find(key);
		if (it != mInputKeys.end() && it->second > 0) {
			return true;
		}

		return false;
	}

	bool KeyBoard::IsKeyHold(const KeyCode& key, U32 frames, bool continuous)const
	{
		if (!IsKeyDown(key)) {
			return false;
		}

		auto it = mInputKeys.find(key);
		return  (it != mInputKeys.end() &&
				(!continuous && it->second == frames) ||
				(continuous && it->second >= frames));
	}

	void KeyBoard::SetKeyState(const KeyCode& key, const KeyState& state)
	{
		using SizeType = std::underlying_type<KeyCode>::type;
		mKeyStates[static_cast<SizeType>(key)] = (state == KeyState::KeyState_Down);
	
		if (state == KeyState_Down)
		{
			auto it = mInputKeys.find(key);
			if (it != mInputKeys.end()) {
				mInputKeys[key] = 0;
			}
		}
	}

	bool KeyBoard::IsKeyDown(const KeyCode& key) const
	{
		using SizeType = std::underlying_type<KeyCode>::type;
		return mKeyStates[static_cast<SizeType>(key)];
	}

	void KeyBoard::Initialize()
	{
	}

	void KeyBoard::Update()
	{
		for (auto it = mInputKeys.begin(); it != mInputKeys.end();)
		{
			KeyCode keyCode = it->first;
			U32 inputSecond = it->second;

			bool toRemoved = false;
			if (IsKeyDown(keyCode)) {
				it->second = inputSecond + 1;
			}
			else {
				toRemoved = true;
			}

			if (toRemoved) {
				mInputKeys.erase(it++);
			}
			else {
				++it;
			}
		}
	}

	void KeyBoard::Uninitialize()
	{
		mKeyStates.reset();
		mInputKeys.clear();
	}

}