#pragma once

#ifdef _WIN32

#include "engine.h"
#include "platform\win32\gameWindowWin32.h"
#include "core\gameComponent.h"
#include "core\settings.h"

namespace Cjing3D {

#define GAME_APP_DEFAULT_ASSET_NAME "Assets"

	class GameAppWin32
	{
	public:
		GameAppWin32(const std::string& assetName = GAME_APP_DEFAULT_ASSET_NAME);
		~GameAppWin32();

		int Run(GameComponent* game);



	private:
		std::unique_ptr<GameWindowWin32> mainWindow = nullptr;
		std::unique_ptr<Engine> mainEngine = nullptr;
		std::string mAssetName;
	};
}

#endif