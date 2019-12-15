#pragma once

#include "common\common.h"
#include "helper\timer.h"
#include "core\systemContext.hpp"
#include "core\gameComponent.h"
#include "renderer\renderableCommon.h"
#include "utils\baseWindow.h"

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

		void SetIsExiting(bool isExiting) { mIsExiting = isExiting; }
		bool GetIsExiting()const { return mIsExiting; }
		void SetHandles(void* windowHwnd, void* windowInstance);
		void SetWindow(BaseWindow* window) { mBaseWindow = window; }

		BaseWindow* GetWindow() { return mBaseWindow; }
		void* GetWindowInstance() { return mWindowHinstance; }
		void* GetWindowHwnd() { return mWindowHwnd; }

		SystemContext& GetGameContext() { return *mSystemContext; }

	private:
		bool mIsInitialized = false;
		bool mIsExiting = false;

		BaseWindow* mBaseWindow;
		void* mWindowHinstance;
		void* mWindowHwnd;

		Timer mTimer;
		EngineTime mTime;

		SystemContext* mSystemContext;
		std::unique_ptr<GameComponent> mGameComponent;

		RenderingDeviceType mRenderingDeviceType;
	};
}