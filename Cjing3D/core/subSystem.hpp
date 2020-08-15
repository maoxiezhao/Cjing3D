#pragma once

#include "common\common.h"

#include <memory>
#include <vector>

namespace Cjing3D
{

class GlobalContext;

class SubSystem : public std::enable_shared_from_this<SubSystem>
{
public:
	SubSystem(GlobalContext& gameContext) : mGameContext(gameContext) {}
	virtual ~SubSystem() = default;

	virtual void Uninitialize() {}
	virtual void Update(F32 deltaTime) {};
	
	GlobalContext& GetGameContext() { return mGameContext; }

protected:
	GlobalContext & mGameContext;
};

}