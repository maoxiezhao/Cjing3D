#include "renderPreset.h"

namespace Cjing3D {

	void RenderPreset::LoadStructuredBuffers()
	{
		auto& device = mDevice;
		// shader light array
		{
			GPUBufferDesc desc = {};
			desc.mUsage = USAGE_DEFAULT;
			desc.mCPUAccessFlags = 0;
			desc.mBindFlags = BIND_SHADER_RESOURCE;
			desc.mMiscFlags = RESOURCE_MISC_BUFFER_STRUCTURED;
			desc.mByteWidth = sizeof(ShaderLight) * SHADER_LIGHT_COUNT;
			desc.mStructureByteStride = sizeof(ShaderLight);

			const auto result = device.CreateBuffer(&desc, mStructuredBuffer[StructuredBufferType_ShaderLight], nullptr);
			Debug::ThrowIfFailed(result, "failed to create shader light buffer:%08x", result);
			device.SetResourceName(mStructuredBuffer[StructuredBufferType_ShaderLight], "ShaderLightArray");
		}
		// shader matrix array
		{
			GPUBufferDesc desc = {};
			desc.mUsage = USAGE_DEFAULT;
			desc.mCPUAccessFlags = 0;
			desc.mBindFlags = BIND_SHADER_RESOURCE;
			desc.mMiscFlags = RESOURCE_MISC_BUFFER_STRUCTURED;
			desc.mByteWidth = sizeof(XMMATRIX) * SHADER_MATRIX_COUNT;
			desc.mStructureByteStride = sizeof(XMMATRIX);

			const auto result = device.CreateBuffer(&desc, mStructuredBuffer[StructuredBufferType_MatrixArray], nullptr);
			Debug::ThrowIfFailed(result, "failed to create shader matrix array buffer:%08x", result);
			device.SetResourceName(mStructuredBuffer[StructuredBufferType_MatrixArray], "ShaderMatrixArray");
		}
	}

}