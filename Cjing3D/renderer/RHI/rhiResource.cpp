#include "rhiResource.h"
#include "device.h"

Cjing3D::GPUResource::~GPUResource()
{
	if (mDevice != nullptr) {
		mDevice->DestroyGPUResource(*this);
	}
}

Cjing3D::RhiTexture2D::~RhiTexture2D()
{
	if (mDevice != nullptr) {
		mDevice->DestroyTexture2D(*this);
	}
}
