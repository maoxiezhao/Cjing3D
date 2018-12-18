#pragma once

#include "world\component\component.h"
#include "renderer\components\model.h"

namespace Cjing3D{

class Renderable : public Component
{
public:
	Renderable(SystemContext& systemContext);
	virtual ~Renderable();

	virtual void Initialize();
	virtual void Uninitialize();
	virtual void Update();

	AABB GetBoundingBox();
	RenderingType GetRenderingType()const { 
		return mRenderingType; 
	}

private:
	//std::shared_ptr<Model> mModel;
	RenderingType mRenderingType;
};

}