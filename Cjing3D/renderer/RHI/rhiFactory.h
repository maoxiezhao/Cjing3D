#pragma once

#include "renderer\RHI\device.h"

namespace Cjing3D
{
	/**************************************************************
	*	\brief Buffer Creating Method
	**************************************************************/
	HRESULT CreateDynamicConstantBuffer(GraphicsDevice & device, GPUBuffer& buffer, size_t dataSize);
	HRESULT CreateDefaultConstantBuffer(GraphicsDevice & device, GPUBuffer& buffer, size_t dataSize);

	// create byte address buffer
	template <typename VertexT>
	HRESULT CreateBABVertexBuffer(
		GraphicsDevice & device, GPUBuffer& buffer, std::vector<VertexT>& vertices, 
		U32 bindFlags = BIND_VERTEX_BUFFER | BIND_SHADER_RESOURCE,
		USAGE usage = USAGE_DEFAULT)
	{
		GPUBufferDesc desc = {};
		desc.mBindFlags = bindFlags; // BIND_VERTEX_BUFFER | BIND_SHADER_RESOURCE
		desc.mByteWidth = static_cast<U32>(vertices.size() * sizeof(VertexT));

		// GPU: read + write
		// CPU: no read + no write
		desc.mUsage = usage; // USAGE_DEFAULT;
		desc.mCPUAccessFlags = 0;

		// Byte Address Buffers
		desc.mMiscFlags = RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
		buffer.SetDesc(desc);

		SubresourceData initiaData = {};
		initiaData.mSysMem = vertices.data();

		return device.CreateBuffer(&desc, buffer, &initiaData);
	}

	template <typename VertexT>
	HRESULT CreateEmptyBABVertexBuffer(
		GraphicsDevice& device, GPUBuffer& buffer, U32 count,
		U32 bindFlags = BIND_VERTEX_BUFFER | BIND_SHADER_RESOURCE,
		USAGE usage = USAGE_DEFAULT)
	{
		GPUBufferDesc desc = {};
		desc.mBindFlags = bindFlags; // BIND_VERTEX_BUFFER | BIND_SHADER_RESOURCE
		desc.mByteWidth = static_cast<U32>(count * sizeof(VertexT));

		// GPU: read + write
		// CPU: no read + no write
		desc.mUsage = usage; // USAGE_DEFAULT;
		desc.mCPUAccessFlags = 0;

		// Byte Address Buffers
		desc.mMiscFlags = RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
		buffer.SetDesc(desc);

		return device.CreateBuffer(&desc, buffer, nullptr);
	}

	template <typename VertexT>
	HRESULT CreateStaticVertexBuffer(GraphicsDevice & device, GPUBuffer& buffer, std::vector<VertexT>& vertices, FORMAT format)
	{
		GPUBufferDesc desc = {};
		desc.mBindFlags = BIND_VERTEX_BUFFER | BIND_SHADER_RESOURCE;
		desc.mByteWidth = static_cast<U32>(vertices.size() * sizeof(VertexT));
		desc.mFormat = format;
		desc.mStructureByteStride = sizeof(VertexT);

		// GPU: read + no write
		// CPU: no read + no write
		desc.mUsage = USAGE_IMMUTABLE;
		desc.mCPUAccessFlags = 0;
		buffer.SetDesc(desc);

		SubresourceData initiaData = {};
		initiaData.mSysMem = vertices.data();

		return device.CreateBuffer(&desc, buffer, &initiaData);
	}

	template <typename T>
	HRESULT CreateStaticShaderBuffer(GraphicsDevice& device, GPUBuffer& buffer, std::vector<T>& vertices, FORMAT format)
	{
		GPUBufferDesc desc = {};
		desc.mBindFlags = BIND_SHADER_RESOURCE;
		desc.mByteWidth = static_cast<U32>(vertices.size() * sizeof(T));
		desc.mFormat = format;
		desc.mStructureByteStride = sizeof(T);

		// GPU: read + no write
		// CPU: no read + no write
		desc.mUsage = USAGE_IMMUTABLE;
		desc.mCPUAccessFlags = 0;
		buffer.SetDesc(desc);

		SubresourceData initiaData = {};
		initiaData.mSysMem = vertices.data();

		return device.CreateBuffer(&desc, buffer, &initiaData);
	}

	template <typename IndexT>
	HRESULT CreateStaticIndexBuffer(GraphicsDevice & device, GPUBuffer& buffer, std::vector<IndexT>& indices)
	{
		GPUBufferDesc desc = {};
		desc.mBindFlags = BIND_INDEX_BUFFER | BIND_SHADER_RESOURCE;
		desc.mByteWidth = static_cast<U32>(indices.size() * sizeof(IndexT));
		desc.mFormat = (sizeof(IndexT) == 4 ? FORMAT_R32_UINT : FORMAT_R16_UINT);
		desc.mStructureByteStride = sizeof(IndexT);

		// GPU: read + no write
		// CPU: no read + no write
		desc.mUsage = USAGE_IMMUTABLE;
		desc.mCPUAccessFlags = 0;
		buffer.SetDesc(desc);

		SubresourceData initiaData = {};
		initiaData.mSysMem = indices.data();

		return device.CreateBuffer(&desc, buffer, &initiaData);
	}

	/**************************************************************
	*	\brief State Creating Method
	**************************************************************/
	HRESULT CreateDefaultSamplerState(GraphicsDevice& device, SamplerState& state, FILTER filter, TEXTURE_ADDRESS_MODE mode, ComparisonFunc func, U32 maxAnisotropy = 0);

	/**************************************************************
	*	\brief Texture Creating Method
	**************************************************************/
	HRESULT CreateDefaultTexture2D(GraphicsDevice& device, Texture2D& texture, U32x2 size, U32 mipMapLevel, BIND_FLAG flag);

	

}