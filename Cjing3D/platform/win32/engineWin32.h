#ifdef _WIN32
#pragma once

#include "app\engine.h"
#include "utils\signal\eventQueue.h"

namespace Cjing3D::Win32
{
	class GameWindowWin32;

	class EngineWin32 : public Engine
	{
	public:
		EngineWin32(const std::shared_ptr<GameWindowWin32>& gameWindow, PresentConfig& config);
		~EngineWin32();

		void Initialize()override;
		void Uninitialize()override;

		void SetAssetPath(const std::string& path, const std::string& name);
		void SetSystemEventQueue(const std::shared_ptr<EventQueue>& eventQueue);
		void HandleSystemMessage(const Event& systemEvent);
		void DoSystemEvents()override;

		std::shared_ptr<JobSystem>		 GetJobSystem()override;
		std::shared_ptr<Settings>	     GetSettings()override;
		std::shared_ptr<InputManager>    GetInputManager()override;
		std::shared_ptr<AudioManager>    GetAudioManager()override;
		std::shared_ptr<ResourceManager> GetResourceMangaer()override;
		std::shared_ptr<GUIStage>        GetGUIStage()override;
		std::shared_ptr<LuaContext>      GetLuaContext()override;

	private:
		std::string mAssetName = "";
		std::string mAssetPath = "Assets";

		bool mIsLockFrameRate = false;
		U32 mTargetFrameRate = 60;
		I32x2 mScreenSize = I32x2(0, 0);
		std::shared_ptr<GameWindowWin32> mGameWindowWin32 = nullptr;

		ScopedConnection mSystemConnection;
		std::shared_ptr<EventQueue> mSystemEventQueue = nullptr;
	};
}


#endif