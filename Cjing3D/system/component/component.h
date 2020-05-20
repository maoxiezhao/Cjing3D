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
	ComponentType_HierarchyComponent,
	ComponentType_Transform,
	ComponentType_Light,
	ComponentType_AABB,
	ComponentType_Name,
	ComponentType_Terrain,
	ComponentType_Animation,
	ComponentType_Armature,
	ComponentType_Weather,
	ComponentType_Custom
};

class Component
{
public:
	Component(ComponentType type);
	Component(const Component& rhs);
	Component(Component&& rhs);
	virtual ~Component();

	ComponentType GetType()const { return mType; }
	void SetType(ComponentType type) { mType = type; }
	Entity GetCurrentEntity()const { return mCurrentEntity; }
	void SetCurrentEntity(Entity entity) { mCurrentEntity = entity; }

	template<typename T>
	static ComponentType DeduceComponentType();

	virtual void Serialize(Archive& archive, U32 seed = 0) {}
	virtual void Unserialize(Archive& archive)const {};

private:
	Entity mCurrentEntity = INVALID_ENTITY;
	ComponentType mType;
	bool mInitialized;
};

using ComponentPtr = std::shared_ptr<Component>;

}