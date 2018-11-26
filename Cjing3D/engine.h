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

		void Initialize();
		void Update();
		void Uninitialize();

		void SetHandles(void* windowHwnd, void* windowInstance);
		void* GetWindowInstance() { return mWindowHinstance; }
		void* GetWindowHwnd() { return mWindowHwnd; }

	private:
		bool mIsInitialized;

		void* mWindowHinstance;
		void* mWindowHwnd;

		Timer mTimer;
		EngineTime mTime;
	};
}