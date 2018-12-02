#pragma once

#include "common\common.h"
#include "helper\timer.h"
#include "core\systemContext.hpp"
#include "core\gameComponent.h"
#include "renderer\renderableCommon.h"

namespace Cjing3D 
{
	class Engine
	{
	public:
		Engine(GameComponent* gameConponent);
		~Engine();
		Engine(const Engine& engine) = delete;
		Engine& operator=(const Engine& engine) = delete;

		void Initialize();
		void Tick();
		void Uninitialize();

		void SetHandles(void* windowHwnd, void* windowInstance);
		void* GetWindowInstance() { return mWindowHinstance; }
		void* GetWindowHwnd() { return mWindowHwnd; }

		SystemContext& GetGameContext() { return *mSystemContext; }

	private:
		bool mIsInitialized;

		void* mWindowHinstance;
		void* mWindowHwnd;

		Timer mTimer;
		EngineTime mTime;

		std::unique_ptr<SystemContext> mSystemContext;
		std::unique_ptr<GameComponent> mGameComponent;

		RenderingDeviceType mRenderingDeviceType;
	};
}