#include "rhiFactory.h"

namespace Cjing3D
{
	HRESULT CreateDynamicConstantBuffer(GraphicsDevice & device, GPUBuffer & buffer, size_t dataSize)
	{
		// CPU可写的Constant Buffer
		GPUBufferDesc desc = {};
		desc.mByteWidth = (U32)dataSize;
		desc.mUsage = USAGE_DYNAMIC;
		desc.mBindFlags = BIND_CONSTANT_BUFFER;
		desc.mCPUAccessFlags = CPU_ACCESS_WRITE;

		buffer.SetDesc(desc);

		return device.CreateBuffer(&desc, buffer, nullptr);
	}

	HRESULT CreateDefaultConstantBuffer(GraphicsDevice & device, GPUBuffer & buffer, size_t dataSize)
	{
		// 仅GPU可读写
		GPUBufferDesc desc = {};
		desc.mByteWidth = (U32)dataSize;
		desc.mUsage = USAGE_DEFAULT;
		desc.mBindFlags = BIND_CONSTANT_BUFFER;

		buffer.SetDesc(desc);

		return device.CreateBuffer(&desc, buffer, nullptr);
	}

	HRESULT CreateDefaultSamplerState(GraphicsDevice & device, SamplerState & state, FILTER filter, TEXTURE_ADDRESS_MODE mode, ComparisonFunc func, U32 maxAnisotropy)
	{
		SamplerDesc desc = {};
		// 过波方式
		desc.mFilter = filter;
		// 纹理寻址模式
		desc.mAddressU = mode;
		desc.mAddressV = mode;
		desc.mAddressW = mode;
		// ??
		desc.mComparisonFunc = func;

		// 各向异性采样数量
		desc.mMaxAnisotropy = maxAnisotropy;

		return device.CreateSamplerState(&desc, state);
	}
}