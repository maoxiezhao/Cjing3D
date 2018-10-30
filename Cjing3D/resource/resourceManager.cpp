#include "resourceManager.h"

namespace Cjing3D
{

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

ResourcePtr ResourceManager::GetOrCreate(const StringID & name, Resource_Type type)
{
	return ResourcePtr();
}

}