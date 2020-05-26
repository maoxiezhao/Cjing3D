#include "resource.h"
#include "resourceManager.h"
#include "renderer\renderableCommon.h"
#include "renderer\RHI\rhiShader.h"

namespace Cjing3D {

template<typename T>
Resource_Type Resource::DeduceResourceType()
{
	return Resource_Unknown;
}

#define ADDTORESOURCETYPE(T, enumT) template<> Resource_Type Resource::DeduceResourceType<T>() { return enumT; }

Resource::Resource(Resource_Type type) :
	mType(type),
	mGUID(GENERATE_ID)
{
}


}
