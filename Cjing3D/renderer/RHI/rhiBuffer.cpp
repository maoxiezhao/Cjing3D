#include "rhiBuffer.h"
#include "renderer\RHI\device.h"
#include "renderer\RHI\rhiFactory.h"

namespace Cjing3D
{
	void GPUBuffer::Create(U32 size)
	{
		HRESULT result = CreateDynamicConstantBuffer(mDevice, *this, size);
		Debug::ThrowIfFailed(result, "Failed to create dynamic constant buffer: %08X.", result);
	}

	void GPUBuffer::UpdateData(const void * data, U32 dataSize)
	{
		mDevice.UpdateBuffer(*this, data, dataSize);
	}
}