#pragma once

#include "renderer\renderableCommon.h"
#include "renderer\renderer.h"
#include "renderer\RHI\rhiResource.h"
#include "renderer\RHI\rhiShader.h"

namespace Cjing3D
{

enum StructuredBufferType
{
	StructuredBufferType_ShaderLight = 0,
	StructuredBufferType_MatrixArray,
	StructuredBufferType_Count,
};

enum ConstantBufferType
{
	ConstantBufferType_Common = 0,
	ConstantBufferType_Camera,
	ConstantBufferType_Frame,
	ConstantBufferType_Image,
	ConstantBufferType_Postprocess,
	ConstantBufferType_MipmapGenerate,
	ConstantBufferType_CubeMap,
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

	inline GPUBuffer& GetStructuredBuffer(StructuredBufferType bufferType){
		return mStructuredBuffer[static_cast<int>(bufferType)];
	}

private:
	void LoadConstantBuffers();
	void LoadStructuredBuffers();

	Renderer & mRenderer;

	GPUBuffer mConstantBuffer[ConstantBufferType_Count];
	GPUBuffer mStructuredBuffer[StructuredBufferType_Count];
};

}