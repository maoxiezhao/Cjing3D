#pragma once

#include "renderer\RHI\device.h"

namespace Cjing3D
{

	HRESULT CreateDynamicConstantBuffer(GraphicsDevice & device, ConstantBuffer& buffer, size_t dataSize);

	HRESULT CreateDefaultSamplerState(GraphicsDevice& device, SamplerState& state, FILTER filter, TEXTURE_ADDRESS_MODE mode, ComparisonFunc func);

}