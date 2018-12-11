#include "rhiTexture.h"
#include "renderer\RHI\device.h"

namespace Cjing3D
{
	GPUResource::GPUResource(GraphicsDevice& device):
		mDevice(device)
	{
	}

	GPUResource::~GPUResource()
	{
		mDevice.DestoryGPUResource(*this);
	}

	RhiTexture::RhiTexture(GraphicsDevice& device) : GPUResource(device) {}
	RhiTexture::~RhiTexture() = default;

	RhiTexture2D::RhiTexture2D(GraphicsDevice& device) : RhiTexture(device) {};
	RhiTexture2D::~RhiTexture2D() = default;

}