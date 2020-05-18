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
		ResourceManager& resourceManager = GlobalGetSubSystem<ResourceManager>();
		mSkyMap = resourceManager.GetOrCreate<RhiTexture2D>(StringID(name));
		mSkyMapName = name;
	}

	void WeatherComponent::Serialize(Archive& archive, U32 seed)
	{
		archive >> mSkyMapName;

		ResourceManager& resourceManager = GlobalGetSubSystem<ResourceManager>();
		if (mSkyMapName.empty() == false)
		{
			mSkyMap = resourceManager.GetOrCreate<RhiTexture2D>(StringID(mSkyMapName));
		}
	}

	void WeatherComponent::Unserialize(Archive& archive) const
	{
		archive << mSkyMapName;
	}
}