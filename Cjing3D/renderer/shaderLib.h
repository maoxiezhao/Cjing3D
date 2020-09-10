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
	void Reload();

	ShaderPtr LoadShader(SHADERSTAGES stages, const std::string& path);
	VertexShaderInfo LoadVertexShaderInfo(const std::string& path, VertexLayoutDesc* desc, U32 numElements);
};

}