#pragma once

template <typename VertexT>
HRESULT CreateStaticVertexBuffer(GraphicsDevice & device, GPUBuffer& buffer, std::vector<VertexT> vertices)
{
	GPUBufferDesc desc = {};
	desc.mBindFlags = BIND_VERTEX_BUFFER;
	desc.mByteWidth = static_cast<U32>(vertices.size() * sizeof(VertexT));

	// GPU: read + no write
	// CPU: no read + no write
	desc.mUsage = D3D11_USAGE_IMMUTABLE;
	desc.mCPUAccessFlags = 0;
	buffer.SetDesc(desc);

	SubresourceData initiaData = {};
	initiaData.mSysMem = vertices.data();

	return device.CreateBuffer(&desc, buffer, nullptr);
}

template <typename IndexT>
HRESULT CreateStaticIndexBuffer(GraphicsDevice & device, GPUBuffer& buffer, std::vector<IndexT> indices)
{
	GPUBufferDesc desc = {};
	desc.mBindFlags = BIND_INDEX_BUFFER;
	desc.mByteWidth = static_cast<U32>(indices.size() * sizeof(IndexT));

	// GPU: read + no write
	// CPU: no read + no write
	desc.mUsage = D3D11_USAGE_IMMUTABLE;
	desc.mCPUAccessFlags = 0;
	buffer.SetDesc(desc);

	SubresourceData initiaData = {};
	initiaData.mSysMem = indices.data();

	return device.CreateBuffer(&desc, buffer, nullptr);
}