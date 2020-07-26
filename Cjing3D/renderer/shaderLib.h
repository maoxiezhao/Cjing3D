#pragma once

#include "renderer\renderableCommon.h"
#include "renderer\renderer.h"
#include "renderer\RHI\rhiResource.h"
#include "renderer\RHI\rhiShader.h"

namespace Cjing3D
{

class ShaderLib
{
public:
	ShaderLib();

	void Initialize();
	void Uninitialize();

	ShaderPtr LoadShader(SHADERSTAGES stages, const std::string& path);
	VertexShaderInfo LoadVertexShaderInfo(const std::string& path, VertexLayoutDesc* desc, U32 numElements);
	InputLayoutPtr LoadInputLayout(ShaderPtr Shader, VertexLayoutDesc* desc, U32 numElements);
	ShaderPtr LoadShader(SHADERSTAGES stages, const void* data, size_t length);

};

}