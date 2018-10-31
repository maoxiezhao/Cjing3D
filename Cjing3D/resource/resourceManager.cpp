#include "resourceManager.h"

namespace Cjing3D
{
void ResourceManager::Initialize()
{
	AddStandardResourceDirectory(Resrouce_VertexShader, "shaders/");
	AddStandardResourceDirectory(Resrouce_PixelShader, "shaders/");
}

void ResourceManager::AddStandardResourceDirectory(Resource_Type type, const std::string & path)
{
	mResourceDirectories[type] = path;
}

const std::string & ResourceManager::GetStandardResourceDirectory(Resource_Type type) const
{
	auto findIt = mResourceDirectories.find(type);
	if (findIt != mResourceDirectories.end())
	{
		return findIt->second;
	}
}

ResourcePtr ResourceManager::GetOrCreateByType(const StringID & name, Resource_Type type)
{
	return ResourcePtr();
}

}