#pragma once

#include "renderer\RHI\device.h"

namespace Cjing3D
{
	HRESULT CreateDynamicConstantBuffer(GraphicsDevice & device, ConstantBuffer& buffer, size_t dataSize);
}