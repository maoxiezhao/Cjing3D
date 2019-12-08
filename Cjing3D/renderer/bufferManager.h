#pragma once

#include "renderer\renderableCommon.h"
#include "renderer\renderer.h"
#include "renderer\RHI\rhiResource.h"
#include "renderer\RHI\rhiShader.h"

namespace Cjing3D
{

enum ResourceBufferType
{
	ResourceBufferType_ShaderLight = 0,
	ResourceBufferType_Count,
};

enum ConstantBufferType
{
	ConstantBufferType_Camera = 0,
	ConstantBufferType_Frame = 1,
	ConstantBufferType_Count,
};

class BufferManager
{
public:
	BufferManager(Renderer& renderer);

	void Initialize();
	void Uninitialize();

	inline GPUBuffer& GetConstantBuffer(ConstantBufferType bufferType) {
		return mConstantBuffer[static_cast<int>(bufferType)];
	}

	inline GPUBuffer& GetResourceBuffer(ResourceBufferType bufferType){
		return mResourceBuffer[static_cast<int>(bufferType)];
	}

private:
	void LoadConstantBuffers();
	void LoadResourceBuffers();

	Renderer & mRenderer;

	GPUBuffer mConstantBuffer[ConstantBufferType_Count];
	GPUBuffer mResourceBuffer[ResourceBufferType_Count];
};

}