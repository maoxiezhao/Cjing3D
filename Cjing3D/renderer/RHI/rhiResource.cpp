#include "rhiResource.h"
#include "device.h"

Cjing3D::GraphicsDeviceChild::~GraphicsDeviceChild()
{
	UnRegister();
}

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

Cjing3D::RasterizerState::~RasterizerState()
{
	if (mDevice != nullptr) {
		mDevice->DestroyRasterizerState(*this);
	}
}

Cjing3D::DepthStencilState::~DepthStencilState()
{
	if (mDevice != nullptr) {
		mDevice->DestroyDepthStencilState(*this);
	}
}

Cjing3D::BlendState::~BlendState()
{
	if (mDevice != nullptr) {
		mDevice->DestroyBlendState(*this);
	}
}

Cjing3D::SamplerState::~SamplerState()
{
	if (mDevice != nullptr) {
		mDevice->DestroySamplerState(*this);
	}
}

Cjing3D::InputLayout::~InputLayout()
{
}
