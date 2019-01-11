#include "rhiBuffer.h"
#include "renderer\RHI\device.h"
#include "renderer\RHI\rhiFactory.h"

namespace Cjing3D
{
	GPUBuffer::GPUBuffer(GraphicsDevice & device):
		GPUResource(device)
	{
	}

	GPUBuffer::~GPUBuffer()
	{
	}

	void GPUBuffer::Create(U32 size)
	{
		auto& device = GetDevice();
		HRESULT result = CreateDynamicConstantBuffer(device, *this, size);
		Debug::ThrowIfFailed(result, "Failed to create dynamic constant buffer: %08X.", result);
	}

	void GPUBuffer::UpdateData(const void * data, U32 dataSize)
	{
		auto& device = GetDevice();
		device.UpdateBuffer(*this, data, dataSize);
	}
}