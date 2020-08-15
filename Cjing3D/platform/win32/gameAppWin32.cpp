#include "gameAppWin32.h"
#include "helper\fileSystem.h"

namespace Cjing3D
{
	GameAppWin32::GameAppWin32()
	{
	}

	GameAppWin32::~GameAppWin32()
	{
	}

	int GameAppWin32::Run(GameComponent* game)
	{
		//initialize window
		const std::string defaultTitle = std::string("Cjing3D") + CjingVersion::GetVersionString();
		mainWindow = std::make_unique<GameWindowWin32>(
			defaultTitle,
			I32x2(DEFAULT_GAME_WINDOW_WIDTH, DEFAULT_GAME_WINDOW_HEIGHT), 
			false
		);
		mainWindow->Show();

		// initialize engine
		mainEngine = std::make_unique<Engine>(game);
		mainEngine->SetHandles(mainWindow->GetHwnd(), mainWindow->GetInstance());
		mainEngine->SetWindow(mainWindow.get());
		mainEngine->Initialize(".", "Assets");

		// run
		mainWindow->RunWindow(*mainEngine);

		// uninitializ
		mainEngine->Uninitialize();
		mainWindow->ShutDown();

		return 0;
	}
}