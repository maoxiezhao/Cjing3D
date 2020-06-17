#pragma once

#include "renderer\renderableCommon.h"
#include "renderer\renderer.h"
#include "renderer\RHI\rhiResource.h"
#include "renderer\RHI\rhiShader.h"

namespace Cjing3D
{

class BufferManager
{
public:
	BufferManager(Renderer& renderer);

	void Initialize();
	void Uninitialize();

	GPUBuffer& GetOrCreateCustomBuffer(const StringID& name);

	inline GPUBuffer& GetConstantBuffer(ConstantBufferType bufferType) {
		return mConstantBuffer[static_cast<int>(bufferType)];
	}

	inline GPUBuffer& GetStructuredBuffer(StructuredBufferType bufferType){
		return mStructuredBuffer[static_cast<int>(bufferType)];
	}

private:

};

}