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

		bool IsVisible()const { return mIsVisible; }
		void SetVisible(bool isVisible);

		void AddGround(const I32x3& pos, U32 tileIndex);
		void RemoveGround(const I32x3& pos);

		GameMapGround* Raycast(const Ray& ray, Scene::PickResult& pickResult);

		virtual void Serialize(JsonArchive& archive);
		virtual void Unserialize(JsonArchive& archive)const;

	private:
		GameMap& mGameMap;
		GameMapGroundInstPtr mInstance = nullptr;

		std::map<I32x3, std::shared_ptr<GameMapGround>> mAllStaticGrounds;
		std::map<I32x3, std::shared_ptr<GameMapDynamicGround>> mAllDynamicGrounds;

		bool mIsVisible = true;
		bool mIsGroundsDirty = false;
	};
}