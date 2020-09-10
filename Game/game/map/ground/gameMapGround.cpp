#include "gameMapGround.h"

namespace CjingGame
{
	GameMapGround::GameMapGround()
	{
	}

	GameMapGround::~GameMapGround()
	{
	}

	void GameMapGround::Serialize(JsonArchive& archive)
	{
		archive.Read("tileIndex", mTileIndex);
	}

	void GameMapGround::Unserialize(JsonArchive& archive)const
	{
		archive.Write("tileIndex", mTileIndex);
	}

	GameMapDynamicGround::GameMapDynamicGround()
	{
	}

	GameMapDynamicGround::~GameMapDynamicGround()
	{
	}
}
