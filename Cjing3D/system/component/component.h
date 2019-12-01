#pragma once

#include "common\common.h"
#include "system\ecsSystem.h"

namespace Cjing3D{

using namespace ECS;

enum ComponentType
{
	ComponentType_Unknown,
	ComponentType_Camera,
	ComponentType_Ojbect,
	ComponentType_Mesh,
	ComponentType_Material,
	ComponentType_Transform,
	ComponentType_Light
};

class Component
{
public:
	Component(ComponentType type);
	virtual ~Component();

	ComponentType GetType()const { return mType; }
	void SetType(ComponentType type) { mType = type; }
	Entity GetCurrentEntity()const { return mCurrentEntity; }
	void SetCurrentEntity(Entity entity) { mCurrentEntity = entity; }

	template<typename T>
	static ComponentType DeduceComponentType();

private:
	Entity mCurrentEntity = INVALID_ENTITY;
	ComponentType mType;
	bool mInitialized;
};

using ComponentPtr = std::shared_ptr<Component>;

}