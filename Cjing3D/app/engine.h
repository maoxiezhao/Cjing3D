#pragma once

#include "common\common.h"

namespace Cjing3D
{
	class JobSystem;
	class Settings;
	class InputManager;
	class AudioManager;
	class ResourceManager;
	class GUIStage;
	class LuaContext;
	class GameWindow;
	struct PresentConfig;

	class Engine : public std::enable_shared_from_this<Engine>
	{
	public:
		Engine(std::shared_ptr<GameWindow> gameWindow, PresentConfig& presentConfig);
		Engine(const Engine& rhs) = delete;
		Engine& operator=(const Engine& rhs) = delete;
		virtual ~Engine() = default;
		
		virtual void Initialize() = 0;
		virtual void Uninitialize() = 0;
		virtual void (DoSystemEvents)() = 0;

		std::shared_ptr<GameWindow> GetGameWindow()const {
			return mGameWindow;
		}

		PresentConfig& GetPresentConfig()const {
			return mPresentConfig;
		}

		virtual std::shared_ptr<JobSystem>		 GetJobSystem() = 0;
		virtual std::shared_ptr<Settings>	     GetSettings() = 0;
		virtual std::shared_ptr<InputManager>    GetInputManager() = 0;
		virtual std::shared_ptr<AudioManager>    GetAudioManager() = 0;
		virtual std::shared_ptr<ResourceManager> GetResourceMangaer() = 0;
		virtual std::shared_ptr<GUIStage>        GetGUIStage() = 0;
		virtual std::shared_ptr<LuaContext>      GetLuaContext() = 0;

		void SetIsExiting(bool isExiting) { mIsExiting = isExiting; }
		bool GetIsExiting()const { return mIsExiting; }

	private:
		std::shared_ptr<GameWindow> mGameWindow = nullptr;
		PresentConfig& mPresentConfig;
		bool mIsExiting = false;
	};
}
