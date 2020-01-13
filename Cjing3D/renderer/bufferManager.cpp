#include "bufferManager.h"

namespace Cjing3D
{
	BufferManager::BufferManager(Renderer & renderer) :
		mRenderer(renderer)
	{
	}

	void BufferManager::Initialize()
	{
		LoadConstantBuffers();
		LoadResourceBuffers();
	}

	void BufferManager::Uninitialize()
	{
	}

	void BufferManager::LoadConstantBuffers()
	{
		auto& device = mRenderer.GetDevice();
		// Camera buffer
		{
			GPUBufferDesc desc = {};
			desc.mUsage = USAGE_DYNAMIC;	// camera buffer ¸üÐÂÆµ·±
			desc.mCPUAccessFlags = CPU_ACCESS_WRITE;
			desc.mBindFlags = BIND_CONSTANT_BUFFER;
			desc.mByteWidth = sizeof(CameraCB);

			const auto result = device.CreateBuffer(&desc, mConstantBuffer[ConstantBufferType_Camera], nullptr);
			Debug::ThrowIfFailed(result, "failed to create camera constant buffer:%08x", result);
			device.SetResourceName(mConstantBuffer[ConstantBufferType_Camera], "CamearCB");
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
		// image buffer
		{
			GPUBufferDesc desc = {};
			desc.mUsage = USAGE_DYNAMIC;
			desc.mCPUAccessFlags = CPU_ACCESS_WRITE;
			desc.mBindFlags = BIND_CONSTANT_BUFFER;
			desc.mByteWidth = sizeof(ImageCB);

			const auto result = device.CreateBuffer(&desc, mConstantBuffer[ConstantBufferType_Image], nullptr);
			Debug::ThrowIfFailed(result, "failed to create frame constant buffer:%08x", result);
			device.SetResourceName(mConstantBuffer[ConstantBufferType_Image], "ImageCB");
		}
	}

	void BufferManager::LoadResourceBuffers()
	{
		auto& device = mRenderer.GetDevice();

		GPUBufferDesc desc = {};
		desc.mUsage = USAGE_DEFAULT;	
		desc.mCPUAccessFlags = 0;
		desc.mBindFlags = BIND_SHADER_RESOURCE;
		desc.mMiscFlags = RESOURCE_MISC_BUFFER_STRUCTURED;
		desc.mByteWidth = sizeof(ShaderLight) * SHADER_LIGHT_COUNT;
		desc.mStructureByteStride = sizeof(ShaderLight);

		const auto result = device.CreateBuffer(&desc, mResourceBuffer[ResourceBufferType_ShaderLight], nullptr);
		Debug::ThrowIfFailed(result, "failed to create shader light buffer:%08x", result);
		device.SetResourceName(mResourceBuffer[ResourceBufferType_ShaderLight], "ShaderLightArray");
	}
}