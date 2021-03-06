#include "weather.h"
#include "resource\resourceManager.h"

namespace Cjing3D
{
	WeatherComponent::WeatherComponent() :
		Component(ComponentType_Weather)
	{
	}

	WeatherComponent::~WeatherComponent()
	{
	}

	void WeatherComponent::LoadSkyMap(const std::string& name)
	{
		auto resourceManager = GetGlobalContext().gResourceManager;
		mSkyMap = resourceManager->GetOrCreate<TextureResource>(StringID(name));
		mSkyMapName = name;
	}

	void WeatherComponent::LoadSkyMap(const I32x2& size, const std::vector<std::filesystem::path>& filePaths)
	{
		auto resourceManager = GetGlobalContext().gResourceManager;
		mSkyMap = resourceManager->GetOrCreateEmptyResource<TextureResource>(StringID(filePaths[0].string()));
		mSkyMapName = filePaths[0].string();
		resourceManager->LoadCubeTextureFromFilePath(filePaths, *mSkyMap->mTexture, size);
	}

	void WeatherComponent::Serialize(Archive& archive, U32 seed)
	{
		archive >> mSkyMapName;

		auto resourceManager = GetGlobalContext().gResourceManager;
		if (mSkyMapName.empty() == false)
		{
			mSkyMap = resourceManager->GetOrCreate<TextureResource>(StringID(mSkyMapName));
		}
	}

	void WeatherComponent::Unserialize(Archive& archive) const
	{
		archive << mSkyMapName;
	}
}