#pragma once

#include "common\common.h"
#include <vector>

namespace Cjing3D
{

class GameContext;

class GameSystem
{
public:
	GameSystem(GameContext& gameContext) :
		mGameContext(gameContext)
	{}

	virtual void Initialize() {};
	virtual void Uninitialize() {};

protected:
	GameContext & mGameContext;
};

}