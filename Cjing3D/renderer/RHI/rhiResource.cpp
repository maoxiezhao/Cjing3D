#include "rhiResource.h"
#include "device.h"

Cjing3D::GPUResource::~GPUResource()
{
	if (mDevice != nullptr) {
		mDevice->DestroyGPUResource(*this);
		mDevice->RemoveGPUResource(this);
	}
}

void Cjing3D::GPUResource::Register(GraphicsDevice * device)
{
	mDevice = device;
	mDevice->AddGPUResource(this);
}

void Cjing3D::GPUResource::UnRegister()
{
	if (mDevice != nullptr) {
		mDevice->DestroyGPUResource(*this);
	}

	GraphicsDeviceChild::UnRegister();
}

Cjing3D::RhiTexture2D::~RhiTexture2D()
{
	if (mDevice != nullptr) {
		mDevice->DestroyTexture2D(*this);
	}
}

void Cjing3D::RhiTexture2D::UnRegister()
{
	if (mDevice != nullptr) {
		mDevice->DestroyTexture2D(*this);
	}

	GPUResource::UnRegister();
}
