#include "gameAppWin32.h"
#include "renderer\presentConfig.h"
#include "platform\win32\gameWindowWin32.h"
#include "platform\win32\engineWin32.h"
#include "app\gameComponent.h"

namespace Cjing3D::Win32
{
	GameAppWin32::GameAppWin32()
	{
	}

	void GameAppWin32::SetInstance(HINSTANCE hInstance)
	{
		mHinstance = hInstance;
	}

	void GameAppWin32::SetAssetPath(const std::string& path, const std::string& name)
	{
		mAssetPath = path;
		mAssetName = name;
	}

	void GameAppWin32::SetTitleName(const UTF8String& string)
	{
		mTitleName = string;
	}

	void GameAppWin32::SetScreenSize(const I32x2& screenSize)
	{
		mScreenSize = screenSize;
	}

	void GameAppWin32::Run(const CreateGameFunc& createGame)
	{
		// initialize memory system
		Memory::Initialize();

#ifdef CJING_DEBUG
		Debug::SetDebugConsoleEnable(true);
		Debug::SetDieOnError(true);
		Debug::InitializeDebugConsole();
#endif
		Logger::PrintConsoleHeader();

		PresentConfig config = {};
		config.mScreenSize = mScreenSize;
		config.mIsFullScreen = false;
		config.mIsLockFrameRate = true;
		config.mTargetFrameRate = 60;
		config.mBackBufferFormat = FORMAT_R8G8B8A8_UNORM;
	
		if (mTitleName == "") {
			mTitleName = std::string("Cjing3D ") + CjingVersion::GetVersionString();
		}

		// system event queue
		std::shared_ptr<EventQueue> eventQueue = CJING_MAKE_SHARED<EventQueue>();

		// create game window
		auto gameWindow = CJING_MAKE_SHARED<GameWindowWin32>(mHinstance, mTitleName, eventQueue, config);

		// create game engine
		auto engine = CJING_MAKE_SHARED<EngineWin32>(gameWindow, config);
		engine->SetAssetPath(mAssetPath, mAssetName);
		engine->SetSystemEventQueue(eventQueue);
		engine->Initialize();

		// create game
		Debug::CheckAssertion(createGame != nullptr);
		auto game = createGame(engine);
		if (game == nullptr) {
			Debug::Die("Failed to create game.");
		}
		game->Initialize();

		// run game
		while (gameWindow->Tick()) 
		{
			if (game->GetIsExiting()) {
				break;
			}
			game->Tick();
		} 

		// uninitialize
		game->Uninitialize();
		engine->Uninitialize();

		game.reset();
		engine.reset();
		gameWindow.reset();
		eventQueue.reset();

#ifdef CJING_DEBUG
		Debug::UninitializeDebugConsolse();
#endif

		// uninitialize memory system
		Memory::Uninitialize();
	}
}