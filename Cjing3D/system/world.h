#pragma once

#include "common\common.h"
#include "core\systemContext.hpp"
#include "core\sceneSystem.h"

namespace Cjing3D{

class World : public SubSystem
{
public:
	World(SystemContext& gameContext);
	virtual ~World();

	virtual void Initialize();
	virtual void Uninitialize();
	virtual void Update();
	virtual void Clear();

	Scene& GetMainScene();
private:

	Scene mMainScene;
};

}