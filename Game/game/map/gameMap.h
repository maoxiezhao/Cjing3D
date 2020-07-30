#pragma once

#include "mapInclude.h"
#include "gameMapGrounds.h"

namespace CjingGame
{
	class GameMap;

	class GameMapPart : public JsonSerializer
	{
	public:
		static const I32 GameMapPartSize = 64;

		GameMapPart(GameMap& gameMap);
		~GameMapPart();

		void Initialize();
		void FixedUpdate();
		void Uninitialize();
		void PreRender();

		bool AddGround(const I32x3& localPos, U32 tileIndex);
		bool RemoveGround(const I32x3& localPos);
		GameMapGrounds* GetGameMapGrounds();

		virtual void Serialize(JsonArchive& archive);
		virtual void Unserialize(JsonArchive& archive)const;

	private:
		GameMap& mGameMap;
		std::unique_ptr<GameMapGrounds> mGameMapGrounds = nullptr;
	};

	class GameMap 
	{
	public:
		static const I32 MapCellSize = 1;

		GameMap(I32 width, I32 height, I32 layer, const std::string& name = "");
		GameMap(U32 mapID);
		GameMap(const std::string& fullPath);
		~GameMap();

		void InitiEmptyMap(I32 width, I32 height, I32 layer);
		void Clear();

		void FixedUpdate();
		void PreRender();

		I32x3 TransformGlobalPosToLocal(const F32x3& pos)const;
		F32x3 TransformLocalPosToGlobal(const I32x3& pos)const;

		// map parts
		GameMapPart* GetCurrentMapPartByGlobalPos(const F32x3& globalPos);
		GameMapPart* GetCurrentMapPartByLocalPos(const I32x3& localPos);

		// getter
		I32 GetMapWidth()const { return mMapWidth; }
		I32 GetMapHeight()const { return mMapHeight; }
		I32 GetMapLayer()const { return mMapLayer; }
		std::string GetMapName()const { return mMapName; }
		U32 GetMapID()const { return mMapID; }

		// setter
		void SetMapName(const std::string& name) { mMapName = name; }

		// ground
		void AddGround(const I32x3& pos, U32 tileIndex);
		void RemoveGround(const I32x3& pos);
		GameMapGrounds* GetGameMapGround();

		// serialize methods
		void LoadMapFromMapID(U32 mapID);
		void LoadMapFromFullPath(const std::string& fullPath);
		void LoadFromFile(const std::string& filePath);
		void SaveToFile(const std::string& filePath);

	private:
		U32 GenerateMapID();
		U32 GetMapIDFromFullPath(const std::string& fullPath)const;
		std::string GetMapFullPathFromID(const std::string& filePath, U32 mapID)const;
		std::string GetRealFileMapNameFromID(U32 mapID)const;

		void LoadMapPart(const std::string& parentPath, const I32x3& localPos);
		void SaveMapPart(const std::string& parentPath, GameMapPart& part);

	private:
		bool mIsLoaded = false;

		// map properties
		U32 mMapID = 0;
		I32 mMapWidth = 0;
		I32 mMapHeight = 0;
		I32 mMapLayer = 0;
		std::string mMapName = "";
		std::string mMapPath = "";

		std::unique_ptr<GameMapPart> mGameMapPart = nullptr;
	};
}