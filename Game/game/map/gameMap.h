#pragma once

#include "core\gameComponent.h"
#include "definitions\definitions.h"
#include "gameMapGrounds.h"

namespace CjingGame
{
	class GameMap;

	class GameMapObjects
	{
	public:
		GameMapObjects(GameMap& gameMap);
		~GameMapObjects();

		void Initialize();
		void FixedUpdate();
		void Uninitialize();
		void PreRender();

		bool AddGround(const I32x3& localPos);
		bool RemoveGround(const I32x3& localPos);

	private:
		GameMap& mGameMap;
		std::unique_ptr<GameMapGrounds> mGameMapGrounds = nullptr;
	};

	class GameMap 
	{
	public:
		static const I32 MapCellSize = 1;

		GameMap();
		~GameMap();

		void Initialize(I32 width, I32 height, I32 layer);
		void Initialize(const std::string& filePath);
		void FixedUpdate();
		void Uninitialize();
		void PreRender();

		// getter\setter
		I32 GetMapWidth()const { return mMapWidth; }
		I32 GetMapHeight()const { return mMapHeight; }
		I32 GetMapLayer()const { return mMapLayer; }

		I32x3 TransformGlobalPosToLocal(const F32x3& pos)const;
		F32x3 TransformLocalPosToGlobal(const I32x3& pos)const;

		void AddGround(const I32x3& pos);
		void RemoveGround(const I32x3& pos);

		// serialize methods
		void LoadFromFile(const std::string& filePath);
		void SaveToFile(const std::string& filePath);

	private:
		bool mIsInitialized = false;
		I32 mMapWidth = 0;
		I32 mMapHeight = 0;
		I32 mMapLayer = 0;

		std::unique_ptr<GameMapObjects> mGameMapObjects = nullptr;
	};
}