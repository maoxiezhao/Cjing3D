#include "shaderLib.h"
#include "renderer.h"

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
	{
		VertexLayoutDesc layout[] = 
		{
			{ "POSITION", 0u, FORMAT_R32G32B32_FLOAT, 0u, APPEND_ALIGNED_ELEMENT,  INPUT_PER_VERTEX_DATA , 0u },
			{ "NORMAL",   0u, FORMAT_R32G32B32_FLOAT, 0u, APPEND_ALIGNED_ELEMENT,  INPUT_PER_VERTEX_DATA , 0u },
			{ "TEXCOORD", 0u, FORMAT_R32G32_FLOAT,    0u, APPEND_ALIGNED_ELEMENT,  INPUT_PER_VERTEX_DATA , 0u }
		};

	}
}

void ShaderLib::LoadPixelShaders()
{
}

void ShaderLib::LoadBuffers()
{
}

}
