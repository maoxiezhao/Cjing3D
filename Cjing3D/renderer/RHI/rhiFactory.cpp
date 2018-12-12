#include "rhiFactory.h"

namespace Cjing3D
{
	HRESULT CreateDynamicConstantBuffer(GraphicsDevice & device, GPUBuffer & buffer, size_t dataSize)
	{
		// CPU��д��Constant Buffer
		GPUBufferDesc desc = {};
		desc.mByteWidth = dataSize;
		desc.mUsage = USAGE_DYNAMIC;		
		desc.mBindFlags = BIND_CONSTANT_BUFFER;
		desc.mCPUAccessFlags = CPU_ACCESS_WRITE;

		buffer.SetDesc(desc);

		return device.CreateBuffer(&desc, buffer, nullptr);
	}

	HRESULT CreateDefaultSamplerState(GraphicsDevice & device, SamplerState & state, FILTER filter, TEXTURE_ADDRESS_MODE mode, ComparisonFunc func)
	{
		SamplerDesc desc = {};
		// ������ʽ
		desc.mFilter = filter;
		// ����Ѱַģʽ
		desc.mAddressU = mode;
		desc.mAddressV = mode;
		desc.mAddressW = mode;
		// ??
		desc.mComparisonFunc = func;

		return device.CreateSamplerState(&desc, state);
	}

}