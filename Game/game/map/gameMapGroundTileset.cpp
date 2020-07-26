#include "gameMapGroundTileset.h"
#include "resource\resourceManager.h"

namespace CjingGame
{
	GameMapGroundTileset::GameMapGroundTileset()
	{
	}

	GameMapGroundTileset::~GameMapGroundTileset()
	{
	}

	void GameMapGroundTileset::LoadTilesetImage(const std::string& path)
	{
		ResourceManager& resourceManager = GlobalGetSubSystem<ResourceManager>();
		mTilesetMap = resourceManager.GetOrCreate<TextureResource>(StringID(path));
		mTilesetPath = path;
	}

	U32x2 GameMapGroundTileset::GetTilesetSize() const
	{
		if (mTilesetMap == nullptr) {
			return U32x2(0u, 0u);
		}

		Texture2D* texture = mTilesetMap->mTexture;
		auto& desc = texture->GetDesc();
		return { desc.mWidth, desc.mHeight };
	}
}