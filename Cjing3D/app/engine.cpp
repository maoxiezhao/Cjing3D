#include "engine.h"
#include "renderer\presentConfig.h"

namespace Cjing3D
{
	Engine::Engine(std::shared_ptr<GameWindow> gameWindow, PresentConfig& presentConfig) :
		mGameWindow(gameWindow),
		mPresentConfig(presentConfig)
	{
	}
}