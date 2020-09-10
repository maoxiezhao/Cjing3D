#pragma once

#include "common\common.h"
#include "core\subSystem.hpp"
#include "core\memory.h"
#include "app\engine.h"
#include "helper\timer.h"

#include <vector>

namespace Cjing3D
{
	class GlobalContext
	{
	public:
		~GlobalContext();

		static GlobalContext& GetInstance()
		{
			static GlobalContext instance;
			return instance;
		}

		std::shared_ptr<Settings>         gSettings = nullptr;
		std::shared_ptr<JobSystem>        gJobSystem = nullptr;
		std::shared_ptr<InputManager>     gInputManager = nullptr;
		std::shared_ptr<AudioManager>     gAudioManager = nullptr;
		std::shared_ptr<ResourceManager>  gResourceManager = nullptr;
		std::shared_ptr<GUIStage>         gGUIStage = nullptr;
		std::shared_ptr<LuaContext>       gLuaContext = nullptr;

		EngineTime mTime = {};
		Timer mEngineTimer;
		std::shared_ptr<Engine> mCurrentEngine = nullptr;

		void Clear();
		F32 GetCurrentTime();
		F32 GetDelatTime();
		void StartTimer();
		void UpdateTimer();
		void StopTimer();
		void SetCurrentEngine(const std::shared_ptr<Engine>& engine);
		std::shared_ptr<Engine> GetEngine();

	private:
		GlobalContext() = default;
	};

	inline GlobalContext& GetGlobalContext()
	{
		return GlobalContext::GetInstance();
	}
}