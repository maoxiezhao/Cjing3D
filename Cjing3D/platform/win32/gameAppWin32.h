#pragma once

#ifdef _WIN32

#include "engine.h"
#include "platform\win32\gameWindowWin32.h"
#include "core\gameComponent.h"
#include "core\settings.h"

namespace Cjing3D {

	class GameAppWin32
	{
	public:
		GameAppWin32();
		~GameAppWin32();

		int Run(GameComponent* game);

	private:
		std::unique_ptr<GameWindowWin32> mainWindow = nullptr;
		std::unique_ptr<Engine> mainEngine = nullptr;
	};
}

#endif