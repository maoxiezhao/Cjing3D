#include "rhiResource.h"
#include "device.h"

Cjing3D::GPUResource::~GPUResource()
{
	mDevice.DestoryGPUResource(*this);
}
