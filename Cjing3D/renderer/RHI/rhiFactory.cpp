#include "rhiFactory.h"

namespace Cjing3D
{
	HRESULT CreateDynamicConstantBuffer(GraphicsDevice & device, GPUBuffer & buffer, size_t dataSize)
	{
		// CPU��д��Constant Buffer
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
		// ��GPU�ɶ�д
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
		// ������ʽ
		desc.mFilter = filter;
		// ����Ѱַģʽ
		desc.mAddressU = mode;
		desc.mAddressV = mode;
		desc.mAddressW = mode;
		// ??
		desc.mComparisonFunc = func;

		// �������Բ�������
		desc.mMaxAnisotropy = maxAnisotropy;

		return device.CreateSamplerState(&desc, state);
	}
}