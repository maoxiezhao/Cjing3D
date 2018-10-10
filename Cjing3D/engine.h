#pragma once

#include "common\common.h"
#include "core\gameComponent.h"
#include "helper\stringID.h"
#include "helper\timer.h"
#include "utils\window.h"

namespace Cjing3D
{
	class Engine
	{
	public:
		Engine();
		~Engine();
		Engine(const Engine& engine) = delete;
		Engine& operator=(const Engine& engine) = delete;

		int Run(std::shared_ptr<GameComponent> gameComponent);

		Window& GetMainWindow();

	private:
		void Initialize();
		void Uninitialize();

	private:
		bool mIsInitialized;

		std::shared_ptr<GameComponent> mGameComponent;

		std::unique_ptr<Window> mMainWindow;

		Timer mTimer;
		EngineTime mTime;
	};
}