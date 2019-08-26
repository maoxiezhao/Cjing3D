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
	virtual ~SubSystem() = default;

	virtual void Update(F32 deltaTime) {};
	SystemContext& GetGameContext() { return mGameContext; }

protected:
	SystemContext & mGameContext;
};

}