#include "renderer\preset\renderPreset.h"

namespace Cjing3D {
	namespace {
		HRESULT CreateWriteDynamicConstantBuffer(GraphicsDevice& device, GPUBuffer& buffer, U32 size)
		{
			GPUBufferDesc desc = {};
			desc.mUsage = USAGE_DYNAMIC;
			desc.mCPUAccessFlags = CPU_ACCESS_WRITE;
			desc.mBindFlags = BIND_CONSTANT_BUFFER;
			desc.mByteWidth = size;

			return device.CreateBuffer(&desc, buffer, nullptr);
		}
	}

	void RenderPreset::LoadConstantBuffers()
	{
		auto& device = mDevice;
		// common buffer
		{
			GPUBufferDesc desc = {};
			desc.mUsage = USAGE_DYNAMIC;	// fast update
			desc.mCPUAccessFlags = CPU_ACCESS_WRITE;
			desc.mBindFlags = BIND_CONSTANT_BUFFER;
			desc.mByteWidth = sizeof(CommonCB);

			const auto result = device.CreateBuffer(&desc, mConstantBuffer[ConstantBufferType_Common], nullptr);
			Debug::ThrowIfFailed(result, "failed to create common constant buffer:%08x", result);
			device.SetResourceName(mConstantBuffer[ConstantBufferType_Common], "CommonCB");
		}
		// Frame buffer
		{
			GPUBufferDesc desc = {};
			desc.mUsage = USAGE_DEFAULT;
			desc.mCPUAccessFlags = 0;
			desc.mBindFlags = BIND_CONSTANT_BUFFER;
			desc.mByteWidth = sizeof(FrameCB);

			const auto result = device.CreateBuffer(&desc, mConstantBuffer[ConstantBufferType_Frame], nullptr);
			Debug::ThrowIfFailed(result, "failed to create frame constant buffer:%08x", result);
			device.SetResourceName(mConstantBuffer[ConstantBufferType_Frame], "FrameCB");
		}
		// Camera buffer
		{
			GPUBuffer& buffer = mConstantBuffer[ConstantBufferType_Camera];
			const HRESULT result = CreateWriteDynamicConstantBuffer(device, buffer, sizeof(CameraCB));
			Debug::ThrowIfFailed(result, "failed to create camera constant buffer:%08x", result);
			device.SetResourceName(buffer, "CameraCB");
		}
		// image buffer
		{
			GPUBuffer& buffer = mConstantBuffer[ConstantBufferType_Image];
			const HRESULT result = CreateWriteDynamicConstantBuffer(device, buffer, sizeof(ImageCB));
			Debug::ThrowIfFailed(result, "failed to create image constant buffer:%08x", result);
			device.SetResourceName(buffer, "ImageCB");
		}
		// postprocess buffer
		{
			GPUBuffer& buffer = mConstantBuffer[ConstantBufferType_Postprocess];
			const HRESULT result = CreateWriteDynamicConstantBuffer(device, buffer, sizeof(PostprocessCB));
			Debug::ThrowIfFailed(result, "failed to create postprocess constant buffer:%08x", result);
			device.SetResourceName(buffer, "PostprocessCB");
		}
		// mipmap generate buffer
		{
			GPUBuffer& buffer = mConstantBuffer[ConstantBufferType_MipmapGenerate];
			const HRESULT result = CreateWriteDynamicConstantBuffer(device, buffer, sizeof(MipmapGenerateCB));
			Debug::ThrowIfFailed(result, "failed to create mipmap generate constant buffer:%08x", result);
			device.SetResourceName(buffer, "MipmapGenerateCB");
		}
		// CubeShadowCB
		{
			GPUBuffer& buffer = mConstantBuffer[ConstantBufferType_CubeMap];
			const HRESULT result = CreateWriteDynamicConstantBuffer(device, buffer, sizeof(CubemapRenderCB));
			Debug::ThrowIfFailed(result, "failed to create CubeShadowCB constant buffer:%08x", result);
			device.SetResourceName(buffer, "CubeShadowCB");
		}
		// CS
		{
			GPUBuffer& buffer = mConstantBuffer[ConstantBufferType_CSParams];
			const HRESULT result = CreateWriteDynamicConstantBuffer(device, buffer, sizeof(CSParamsCB));
			Debug::ThrowIfFailed(result, "failed to create CSParamsCB constant buffer:%08x", result);
			device.SetResourceName(buffer, "CSParamsCB");
		}
		// Misc
		{
			GPUBuffer& buffer = mConstantBuffer[ConstantBufferType_Misc];
			const HRESULT result = CreateWriteDynamicConstantBuffer(device, buffer, sizeof(RenderMiscCB));
			Debug::ThrowIfFailed(result, "failed to create RenderMiscCB constant buffer:%08x", result);
			device.SetResourceName(buffer, "RenderMiscCB");	
		}
	}
}