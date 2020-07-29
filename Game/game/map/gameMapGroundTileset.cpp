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

		auto texture = mTilesetMap->mTexture;
		if (texture != nullptr)
		{
			mTileColCount = texture->GetDesc().mWidth  / mTileCellSize;
			mTileRowCount = texture->GetDesc().mHeight / mTileCellSize;
		}
		
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

	U32x4 GameMapGroundTileset::GetTileRectByIndex(U32 index) const
	{
		U32x2 pos = Flatten(index);
		return U32x4(
			pos[0] * mTileCellSize,
			pos[1] * mTileCellSize,
			mTileCellSize,
			mTileCellSize
		);
	}

	U32x2 GameMapGroundTileset::Flatten(U32 index) const
	{
		return mTileColCount > 0 && mTileRowCount > 0 ?
			U32x2(index % mTileColCount, index / mTileColCount) :
			U32x2(0u, 0u);
	}

	U32 GameMapGroundTileset::UnFlatten(const U32x2& pos) const
	{
		return pos[1] * mTileColCount + pos[0];
	}
}