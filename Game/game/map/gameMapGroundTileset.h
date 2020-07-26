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

	private:
		std::string mTilesetPath;
		TextureResourcePtr mTilesetMap = nullptr;
	};
}