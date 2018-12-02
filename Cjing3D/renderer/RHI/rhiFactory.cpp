#include "rhiFactory.h"

namespace Cjing3D
{

	HRESULT CreateDynamicConstantBuffer(GraphicsDevice & device, ConstantBuffer & buffer, size_t dataSize)
	{
		GPUBufferDesc desc = {};
		desc.mByteWidth = dataSize;
		desc.mUsage = USAGE_DYNAMIC;
		desc.mBindFlags = BIND_CONSTANT_BUFFER;
		desc.mCPUAccessFlags = CPU_ACCESS_WRITE;

		buffer.SetDesc(desc);

		return device.CreateBuffer(&desc, buffer);
	}

}