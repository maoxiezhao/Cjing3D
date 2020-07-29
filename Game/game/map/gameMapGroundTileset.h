#pragma once

#include "definitions\definitions.h"
#include "resource\resource.h"

namespace CjingGame
{
	class GameMapGroundTileset
	{
	public:
		GameMapGroundTileset();
		~GameMapGroundTileset();

		void LoadTilesetImage(const std::string& path);
		const std::string GetBaseColorTilesetPath() const { return mTilesetPath; }
		TextureResourcePtr GetBaseColorTexture() { return mTilesetMap; }
		U32x2 GetTilesetSize()const;
		U32x4 GetTileRectByIndex(U32 index)const;

		U32x2 Flatten(U32 index)const;
		U32 UnFlatten(const U32x2& pos)const;

	private:
		std::string mTilesetPath;
		TextureResourcePtr mTilesetMap = nullptr;
		U32 mTileCellSize = 16;
		U32 mTileColCount = 0;
		U32 mTileRowCount = 0;
	};
}