#pragma once

#include "common\common.h"
#include "core\subSystem.hpp"
#include "core\memory.h"
#include "helper\timer.h"

#include <vector>

namespace Cjing3D
{

class Engine;

template<typename T>
constexpr void CheckSubSystemType()
{
	static_assert(std::is_base_of<SubSystem, T>::value, "The T must implement Subystem");
}

class GlobalContext
{
private:
	using SubSystemPtr = std::shared_ptr<SubSystem>;
	std::vector<SubSystemPtr> mSubSystems;
	
	EngineTime mTime = {};
	Engine* mEngine = nullptr;

	GlobalContext() {};

public:
	~GlobalContext() 
	{ 
		for (auto it = mSubSystems.rbegin(); it != mSubSystems.rend(); it++) {
			(*it).reset();
		}
		mSubSystems.clear();
	}

	static GlobalContext& GetGlobalContext()
	{
		static GlobalContext instance;
		return instance;
	}

	template<typename T, typename... Args>
	void RegisterSubSystem(Args&&... args)
	{
		CheckSubSystemType<T>();
	
		auto instant = std::make_shared<T>(*this);
		mSubSystems.push_back(instant);
		instant->Initialize(std::forward<Args>(args)...);
	}

	template <typename T>
	T& GetSubSystem()
	{
		CheckSubSystemType<T>();

		for (const auto& subSystem : mSubSystems)
		{
			if (typeid(T) == typeid(*subSystem)) {
				return (static_cast<T&>(*subSystem.get()));
			}
		}
	}

	void Uninitialize()
	{
		for (auto it = mSubSystems.rbegin(); it != mSubSystems.rend(); it++) {
			(*it)->Uninitialize();
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
	return GlobalContext::GetGlobalContext().GetSubSystem<T>();
}

inline F32 GlobalGetCurrentTime()
{
	return GlobalContext::GetGlobalContext().GetCurrentTime();
}

inline F32 GlobalGetDeltaTime()
{
	return GlobalContext::GetGlobalContext().GetDelatTime();
}

inline Engine* GlobalGetEngine()
{
	return GlobalContext::GetGlobalContext().GetEngine();
}

}