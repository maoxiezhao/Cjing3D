#pragma once

#include "common\common.h"
#include "core\subSystem.hpp"
#include "helper\timer.h"

#include <vector>

namespace Cjing3D
{

class SystemContext
{
private:
	using GameSystemPtr = std::unique_ptr<SubSystem>;
	std::vector<GameSystemPtr> mSystems;
	EngineTime mTime;

	SystemContext() = default;

public:
	~SystemContext() { mSystems.clear();}

	static SystemContext& GetSystemContext()
	{
		static SystemContext instance;
		return instance;
	}

	void RegisterSubSystem(SubSystem* gameSystem)
	{
		if (gameSystem == nullptr) {
			return;
		}

		auto gameSystemPtr = std::unique_ptr<SubSystem>(gameSystem);
		mSystems.push_back(std::move(gameSystemPtr));
	}

	template <typename T>
	T& GetSubSystem()
	{
		for (const auto& subSystem : mSystems)
		{
			if (typeid(T) == typeid(*subSystem)) {
				return (dynamic_cast<T&>(*subSystem.get()));
			}
		}
	}

	void SetEngineTime(EngineTime time)
	{
		mTime = time;
	}

	EngineTime GetEngineTime()
	{
		return mTime;
	}
};

}