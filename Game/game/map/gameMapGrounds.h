#pragma once

#include "definitions\definitions.h"
#include "game\map\GameMapGroundRenderer.h"

namespace CjingGame
{
	class GameMap;

	class GameMapGrounds
	{
	public:
		GameMapGrounds(GameMap& gameMap);
		~GameMapGrounds();

		void Initialize();
		void FixedUpdate();
		void Uninitialize();
		void PreRender();

		void AddGround(const I32x3& pos, U32 tileIndex);
		void RemoveGround(const I32x3& pos);

		GameMapGroundTileset& GetTileset() { return mGroundTileset; }

	private:
		GameMap& mGameMap;
		GameMapGroundRenderer mGroundRenderer;
		std::map<I32x3, std::shared_ptr<GameMapGround>> mAllGrounds;
		GameMapGroundTileset mGroundTileset;
	};
}