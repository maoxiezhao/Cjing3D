#pragma once

#include "common\common.h"
#include "core\gameContext.hpp"

namespace Cjing3D{

class World : public GameSystem
{
public:
	World(GameContext& gameContext);
	virtual ~World();

	virtual void Initialize();
	virtual void Uninitialize();

private:

};

}