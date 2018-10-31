#include "shaderLib.h"
#include "renderer.h"
#include "resource\resourceManager.h"
#include "renderer\RHI\rhiShader.h"

namespace Cjing3D
{

ShaderLib::ShaderLib(Renderer & renderer) :
	mRenderer(renderer)
{
}

void ShaderLib::Initialize()
{
	LoadBuffers();
	LoadShader();
}

void ShaderLib::Uninitialize()
{
}

void ShaderLib::LoadShader()
{
	LoadVertexShaders();
	LoadPixelShaders();
}

void ShaderLib::LoadVertexShaders()
{
	auto& resourceManager = mRenderer.GetResourceManager();
	const std::string shaderPath = resourceManager.GetStandardResourceDirectory(Resrouce_VertexShader);
	{
		VertexLayoutDesc layout[] = 
		{
			{ "POSITION", 0u, FORMAT_R32G32B32_FLOAT, 0u, APPEND_ALIGNED_ELEMENT,  INPUT_PER_VERTEX_DATA , 0u },
			{ "NORMAL",   0u, FORMAT_R32G32B32_FLOAT, 0u, APPEND_ALIGNED_ELEMENT,  INPUT_PER_VERTEX_DATA , 0u },
			{ "TEXCOORD", 0u, FORMAT_R32G32_FLOAT,    0u, APPEND_ALIGNED_ELEMENT,  INPUT_PER_VERTEX_DATA , 0u }
		};
		auto vsinfo = resourceManager.GetOrCreate<VertexShaderInfo>(shaderPath + "objectVS.cso", layout, 3);
	}
}

void ShaderLib::LoadPixelShaders()
{
}

void ShaderLib::LoadBuffers()
{
}

}
