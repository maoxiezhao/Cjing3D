#pragma once

#include "world\component\component.h"
#include "core\gameContext.hpp"

namespace Cjing3D{

class Renderable : public Component
{
public:
	Renderable(GameContext& gameContext);
	virtual ~Renderable();

	virtual void Initialize();
	virtual void Uninitialize();
	virtual void Update();
};

}