#include "component.h"

namespace Cjing3D
{
template<typename T>
ComponentType Component::DeduceComponentType()
{
	return ComponentType_Unknown;
}

#define ADDTOCOMPONENTTYPE(T, enumT) template<> ComponentType Component::DeduceComponentType<T>() { return enumT; }
//ADDTOCOMPONENTTYPE(Renderable, ComponentType_Renderable)
	
Component::Component(ComponentType type):
	mType(type),
	mInitialized(false)
{
}

Component::Component(const Component& rhs)
{
	mCurrentEntity = rhs.mCurrentEntity;
	mType = rhs.mType;
	mInitialized = rhs.mInitialized;
}

Component::Component(Component&& rhs)
{
	mCurrentEntity = rhs.mCurrentEntity;
	rhs.mCurrentEntity = INVALID_ENTITY;

	mType = rhs.mType;
	mInitialized = rhs.mInitialized;
}

Component::~Component()
{
}


}