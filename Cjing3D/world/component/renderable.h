#pragma once

#include "world\component\component.h"
#include "core\gameContext.hpp"
#include "renderer\components\model.h"

namespace Cjing3D{

class Renderable : public Component
{
public:
	Renderable();
	Renderable(GameContext& gameContext);
	virtual ~Renderable();

	virtual void Initialize();
	virtual void Uninitialize();
	virtual void Update();

private:
	std::shared_ptr<Model> mModel;

};

}