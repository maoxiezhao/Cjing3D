#pragma once

#include "common\common.h"
#include "core\gameContext.hpp"

namespace Cjing3D{

enum ComponentType
{
	ComponentType_Unknown
};

class Component
{
public:
	Component();
	Component(Component&& rhs);
	Component(const Component& rhs) = delete;

	Component& operator=(const Component& component) = delete;

	virtual ~Component();

	virtual void Initialize();
	virtual void Uninitialize();
	virtual void Update();

	ComponentType GetType()const { return mType; }
	U32 GetGUID()const { return mGUID; }

private:
	U32 mGUID;
	ComponentType mType;

};

using ComponentPtr = std::shared_ptr<Component>;

}