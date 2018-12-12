#pragma once

#include "renderer\RHI\device.h"

namespace Cjing3D
{
	/**************************************************************
	*	\brief Buffer Creating Method
	**************************************************************/
	HRESULT CreateDynamicConstantBuffer(GraphicsDevice & device, GPUBuffer& buffer, size_t dataSize);

	template <typename VertexT>
	HRESULT CreateStaticVertexBuffer(GraphicsDevice & device, GPUBuffer& buffer, std::vector<VertexT> vertices);

	template <typename IndexT>
	HRESULT CreateStaticIndexBuffer(GraphicsDevice & device, GPUBuffer& buffer, std::vector<IndexT> indices);

	/**************************************************************
	*	\brief State Creating Method
	**************************************************************/
	HRESULT CreateDefaultSamplerState(GraphicsDevice& device, SamplerState& state, FILTER filter, TEXTURE_ADDRESS_MODE mode, ComparisonFunc func);

#include "renderer\RHI\rhiFactory.h"
}