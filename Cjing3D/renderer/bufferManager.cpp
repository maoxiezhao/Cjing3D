#include "bufferManager.h"

namespace Cjing3D
{
	namespace {
		HRESULT CreateWriteDynamicConstantBuffer(GraphicsDevice&device, GPUBuffer& buffer, U32 size)
		{
			GPUBufferDesc desc = {};
			desc.mUsage = USAGE_DYNAMIC;
			desc.mCPUAccessFlags = CPU_ACCESS_WRITE;
			desc.mBindFlags = BIND_CONSTANT_BUFFER;
			desc.mByteWidth = size;

			return device.CreateBuffer(&desc, buffer, nullptr);
		}
	
	}

	BufferManager::BufferManager(Renderer & renderer) :
		mRenderer(renderer)
	{
	}

	void BufferManager::Initialize()
	{
		LoadConstantBuffers();
		LoadStructuredBuffers();
	}

	void BufferManager::Uninitialize()
	{
		for (U32 i = 0; i < ConstantBufferType_Count; i++) {
			mConstantBuffer[i].Clear();
		}
		for (U32 i = 0; i < StructuredBufferType_Count; i++) {
			mStructuredBuffer[i].Clear();
		}
	}

	void BufferManager::LoadConstantBuffers()
	{
		auto& device = mRenderer.GetDevice();
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
	}

	void BufferManager::LoadStructuredBuffers()
	{
		auto& device = mRenderer.GetDevice();

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