#include "component.h"
#include "world\component\renderable.h"

namespace Cjing3D
{
template<typename T>
Component_Type Component::DeduceComponentType()
{
	return ComponentType_Unknown;
}

#define ADDTOCOMPONENTTYPE(T, enumT) template<> Component_Type Component::DeduceComponentType<T>() { return enumT; }
ADDTOCOMPONENTTYPE(Renderable, ComponentType_Renderable)

	
Component::Component(SystemContext& systemContext):
	mSystemContext(systemContext),
	mInitialized(false)
{
}

Component::Component(Component && rhs):
	mSystemContext(rhs.mSystemContext)
{
}

Component::~Component()
{
}

void Component::Initialize()
{
	if (mInitialized == true) {
		return;
	}
}

void Component::Uninitialize()
{
	if (mInitialized == false) {
		return;
	}
}

void Component::Update()
{
}

}