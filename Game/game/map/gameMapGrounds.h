#pragma once

#include "game\map\gameMapGround.h"
#include "game\map\GameMapGroundRenderer.h"

namespace CjingGame
{
	class GameMap;

	class GameMapGrounds : public JsonSerializer
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

		virtual void Serialize(JsonArchive& archive);
		virtual void Unserialize(JsonArchive& archive)const;

	private:
		GameMap& mGameMap;
		GameMapGroundInstPtr mInstance = nullptr;

		std::map<I32x3, std::shared_ptr<GameMapGround>> mAllStaticGrounds;
		std::map<I32x3, std::shared_ptr<GameMapDynamicGround>> mAllDynamicGrounds;

		bool mIsGroundsDirty = false;
	};
}