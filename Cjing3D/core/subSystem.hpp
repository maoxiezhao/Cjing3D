#pragma once

#include "common\common.h"
#include "core\threadSafeSystem.h"
#include "core\eventSystem.h"

#include <vector>

namespace Cjing3D
{

class SystemContext;

class SubSystem : public ThreadSafeSystem
{
public:
	SubSystem(SystemContext& gameContext) :
		mGameContext(gameContext)
	{}

	virtual void Initialize() {};
	virtual void Uninitialize() {};

	SystemContext& GetGameContext() { return mGameContext; }

protected:
	SystemContext & mGameContext;
};

}