#include "terrain.h"
#include "resource\resourceManager.h"

namespace Cjing3D
{
	TerrainComponent::TerrainComponent() :
		Component(ComponentType_Terrain)
	{
	}
	TerrainComponent::~TerrainComponent()
	{
	}

	void TerrainComponent::SetElevation(U32 elevation)
	{
		mTerrainElevation = elevation;

		SetTerrainDirty(true);
	}

	U32 TerrainComponent::GetElevation() const
	{
		return mTerrainElevation;
	}

	void TerrainComponent::SetTerrainSize(U32 width, U32 height)
	{
		mTerrainWidth = width;
		mTerrainHeight = height;

		SetTerrainDirty(true);
	}

	void TerrainComponent::LoadHeightMap(const std::string& name)
	{
		ResourceManager& resourceManager = GlobalGetSubSystem<ResourceManager>();
		mHeightMap = resourceManager.GetOrCreate<RhiTexture2D>(StringID(name), FORMAT_R8_UNORM, 1);
		mHeightMapName = name;

		SetTerrainDirty(true);
	}

	Texture2DPtr TerrainComponent::GetHeightMap()
	{
		return mHeightMap;
	}

	void TerrainComponent::LoadDetailMap(const std::string& weightMapName, const std::string& detailName1, const std::string& detailName2, const std::string& detailName3)
	{
		ResourceManager& resourceManager = GlobalGetSubSystem<ResourceManager>();
		mMaterial.weightTexture  = resourceManager.GetOrCreate<RhiTexture2D>(weightMapName, FORMAT_R8G8B8A8_UNORM, 1);
		mMaterial.detailTexture1 = resourceManager.GetOrCreate<RhiTexture2D>(detailName1,   FORMAT_R8G8B8A8_UNORM, 1);
		mMaterial.detailTexture2 = resourceManager.GetOrCreate<RhiTexture2D>(detailName2,   FORMAT_R8G8B8A8_UNORM, 1);
		mMaterial.detailTexture3 = resourceManager.GetOrCreate<RhiTexture2D>(detailName3,   FORMAT_R8G8B8A8_UNORM, 1);

		mWeightTextureName = weightMapName;
		mDetailTextureName1 = detailName1;
		mDetailTextureName2 = detailName2;
		mDetailTextureName3 = detailName3;

		SetTerrainDirty(true);
	}

	TerrainMaterial& TerrainComponent::GetTextureMaterial()
	{
		return mMaterial;
	}

	void TerrainComponent::Serialize(Archive& archive, U32 seed)
	{
		archive >> mTerrainWidth;
		archive >> mTerrainHeight;
		archive >> mTerrainElevation;

		archive >> mHeightMapName;
		archive >> mWeightTextureName;
		archive >> mDetailTextureName1;
		archive >> mDetailTextureName2;
		archive >> mDetailTextureName3;

		ResourceManager& resourceManager = GlobalGetSubSystem<ResourceManager>();

		if (mHeightMapName.empty() == false)
		{
			mHeightMap = resourceManager.GetOrCreate<RhiTexture2D>(StringID(mHeightMapName), FORMAT_R8_UNORM, 1);
		}
		if (mWeightTextureName.empty() == false)
		{
			mMaterial.weightTexture = resourceManager.GetOrCreate<RhiTexture2D>(StringID(mWeightTextureName), FORMAT_R8_UNORM, 1);
		}
		if (mDetailTextureName1.empty() == false)
		{
			mMaterial.detailTexture1 = resourceManager.GetOrCreate<RhiTexture2D>(StringID(mDetailTextureName1), FORMAT_R8_UNORM, 1);
		}
		if (mDetailTextureName2.empty() == false)
		{
			mMaterial.detailTexture2 = resourceManager.GetOrCreate<RhiTexture2D>(StringID(mDetailTextureName2), FORMAT_R8_UNORM, 1);
		}
		if (mDetailTextureName3.empty() == false)
		{
			mMaterial.detailTexture3 = resourceManager.GetOrCreate<RhiTexture2D>(StringID(mDetailTextureName3), FORMAT_R8_UNORM, 1);
		}

		SetTerrainDirty(true);
	}

	void TerrainComponent::Unserialize(Archive& archive) const
	{
		archive << mTerrainWidth;
		archive << mTerrainHeight;
		archive << mTerrainElevation;

		archive << mHeightMapName;
		archive << mWeightTextureName;
		archive << mDetailTextureName1;
		archive << mDetailTextureName2;
		archive << mDetailTextureName3;
	}
}