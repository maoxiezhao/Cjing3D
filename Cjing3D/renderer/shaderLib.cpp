#include "shaderLib.h"

namespace Cjing3D
{

ResourceLoader::ResourceLoader(Renderer & renderer):
	mRenderer(renderer)
{
}

void ResourceLoader::Initialize()
{
	LoadShader();
	LoadBuffers();
}

void ResourceLoader::Uninitialize()
{
}

void ResourceLoader::LoadShader()
{
}

void ResourceLoader::LoadBuffers()
{
}

}
