#pragma once

#include "common\common.h"
#include "helper\timer.h"

namespace Cjing3D
{
	class Engine
	{
	public:
		Engine();
		~Engine();
		Engine(const Engine& engine) = delete;
		Engine& operator=(const Engine& engine) = delete;

		//int Run(std::shared_ptr<GameComponent> gameComponent);

		void Initialize();
		void Update();
		void Uninitialize();

	private:
		bool mIsInitialized;

		//std::unique_ptr<Window> mMainWindow;

		Timer mTimer;
		EngineTime mTime;
	};
}