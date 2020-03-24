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

ADDTORESOURCETYPE(VertexShaderInfo, Resrouce_VertexShader)
ADDTORESOURCETYPE(VertexShader, Resrouce_VertexShader)
ADDTORESOURCETYPE(PixelShader, Resrouce_PixelShader)
ADDTORESOURCETYPE(ComputeShader, Resource_ComputeShader);
ADDTORESOURCETYPE(HullShader, Resource_HullShader);
ADDTORESOURCETYPE(DomainShader, Resource_DomainShader);

Resource::Resource(Resource_Type type) :
	mType(type),
	mGUID(GENERATE_ID)
{
}


}
