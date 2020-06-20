#pragma once

#include "common\common.h"
#include "core\subSystem.hpp"
#include "helper\timer.h"

#include <vector>

namespace Cjing3D
{

class Engine;

class SystemContext
{
private:
	using GameSystemPtr = std::unique_ptr<SubSystem>;
	std::vector<GameSystemPtr> mSystems;
	
	EngineTime mTime = {};
	Engine* mEngine = nullptr;

	SystemContext() {};

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

	F32 GetCurrentTime()
	{
		return F32(mTime.totalDeltaTime / 1000.0f);
	}

	F32 GetDelatTime()
	{
		return F32(mTime.deltaTime / 1000.0f);
	}

	void SetEngine(Engine* engine)
	{
		mEngine = engine;
	}

	Engine* GetEngine()
	{
		return mEngine;
	}
};

template <typename T>
inline T& GlobalGetSubSystem()
{
	return SystemContext::GetSystemContext().GetSubSystem<T>();
}

inline F32 GlobalGetCurrentTime()
{
	return SystemContext::GetSystemContext().GetCurrentTime();
}

inline F32 GlobalGetDeltaTime()
{
	return SystemContext::GetSystemContext().GetDelatTime();
}

inline Engine* GlobalGetEngine()
{
	return SystemContext::GetSystemContext().GetEngine();
}

}