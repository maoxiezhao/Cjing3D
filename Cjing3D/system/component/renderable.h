#pragma once

#include "system\component\component.h"

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
	ShaderType GetRenderingType()const {
		return mRenderingType; 
	}

private:
	//std::shared_ptr<Model> mModel;
	ShaderType mRenderingType;
};

}