#include "rhiTexture.h"
#include "renderer\RHI\device.h"

namespace Cjing3D
{
	RhiTexture::RhiTexture(GraphicsDevice& device) : GPUResource(device) {}
	RhiTexture::~RhiTexture() = default;

	RhiTexture2D::RhiTexture2D(GraphicsDevice& device) : RhiTexture(device) {};
	RhiTexture2D::~RhiTexture2D() = default;

}