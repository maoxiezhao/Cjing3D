#pragma once

#include "common\common.h"
#include "core\threadSafeSystem.h"

#include <vector>

namespace Cjing3D
{

class GameContext;

class GameSystem : public ThreadSafeSystem
{
public:
	GameSystem(GameContext& gameContext) :
		mGameContext(gameContext)
	{}

	virtual void Initialize() {};
	virtual void Uninitialize() {};

	GameContext& GetGameContext() { return mGameContext; }

protected:
	GameContext & mGameContext;
};

}