#pragma once

#include "common\common.h"
#include "core\gameSystem.hpp"
#include <vector>

namespace Cjing3D
{

class GameContext
{
private:
	using GameSystemPtr = std::unique_ptr<GameSystem>;
	std::vector<GameSystemPtr> mGameSystems;

public:
	GameContext() = default;
	~GameContext()
	{
		mGameSystems.clear();
	}

	void RegisterGameSystem(GameSystem* gameSystem)
	{
		if (gameSystem == nullptr) {
			return;
		}

		auto gameSystemPtr = std::unique_ptr<GameSystem>(gameSystem);
		mGameSystems.push_back(std::move(gameSystemPtr));
	}

	template <typename T>
	T& GetGameSystem()
	{
		for (const auto& gameSystem : mGameSystems)
		{
			if (typeid(T) == typeid(*gameSystem)) {
				return (dynamic_cast<T&>(*gameSystem.get()));
			}
		}
	}
};

}