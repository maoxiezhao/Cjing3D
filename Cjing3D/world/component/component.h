#pragma once

#include "common\common.h"
#include "core\systemContext.hpp"

namespace Cjing3D{

class Transform;

enum Component_Type
{
	ComponentType_Unknown,
	ComponentType_Renderable,
	ComponentType_Transform,
};

class Component
{
public:
	Component(SystemContext& systemContext);
	Component(Component&& rhs);
	Component(const Component& rhs) = delete;

	Component& operator=(const Component& component) = delete;

	virtual ~Component();

	virtual void Initialize();
	virtual void Uninitialize();
	virtual void Update();

	Component_Type GetType()const { return mType; }
	void SetType(Component_Type type) { mType = type; }
	U32 GetGUID()const { return mGUID; }

	template<typename T>
	static Component_Type DeduceComponentType();

private:
	U32 mGUID;
	Component_Type mType;
	bool mInitialized;
	SystemContext& mSystemContext;
};

using ComponentPtr = std::shared_ptr<Component>;

}