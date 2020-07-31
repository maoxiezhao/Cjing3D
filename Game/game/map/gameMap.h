#pragma once

#include "mapInclude.h"
#include "gameMapGrounds.h"
#include "mapTileset.h"

namespace CjingGame
{
	class GameMap;

	struct MapPartPosition
	{
	public:
		MapPartPosition(I32 xx, I32 yy, I32 zz) : x(xx), y(yy), z(zz) {}
		I32 x = 0;
		I32 y = 0;
		I32 z = 0;

		static MapPartPosition TransformFromLocalPos(const I32x3& localPos);
	};

	class GameMapPart : public JsonSerializer
	{
	public:
		GameMapPart(GameMap& gameMap);
		~GameMapPart();

		void Initialize();
		void FixedUpdate();
		void Uninitialize();
		void PreRender();

		bool IsVisible()const { return mIsVisible; }
		void SetVisible(bool isVisible);

		bool AddGround(const I32x3& localPos, U32 tileIndex);
		bool RemoveGround(const I32x3& localPos);
		GameMapGrounds* GetGameMapGrounds();

		virtual void Serialize(JsonArchive& archive);
		virtual void Unserialize(JsonArchive& archive)const;

	private:
		GameMap& mGameMap;
		bool mIsVisible = true;

		std::unique_ptr<GameMapGrounds> mGameMapGrounds = nullptr;
	};

	class GameMap 
	{
	public:
		using GameMapPartPtr = std::shared_ptr<GameMapPart>;

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
		void LoadMapParts(const MapPartPosition& defaultPartPos);
		void LoadMapPart(const MapPartPosition& mapPartPos);
		void RefreshMapParts(const MapPartPosition& localPos);
		void SetMapPartByPartPos(const MapPartPosition& pos, GameMapPartPtr mapPart);
		void ClearAllMapParts();
		U32 GetMapPartIndexByPartPos(const MapPartPosition& localPos)const;
		U32 GetMapPartSize()const;

		GameMapPart* GetCurrentMapPartByGlobalPos(const F32x3& globalPos);
		GameMapPart* GetCurrentMapPartByLocalPos(const I32x3& localPos);

		// getter
		I32 GetMapWidth()const { return mMapWidth; }
		I32 GetMapHeight()const { return mMapHeight; }
		I32 GetMapLayer()const { return mMapLayer; }
		std::string GetMapName()const { return mMapName; }
		U32 GetMapID()const { return mMapID; }
		MapTileset& GetMapTileset() { return mMapTilset; }

		// setter
		void SetMapName(const std::string& name) { mMapName = name; }

		// ground
		void AddGround(const I32x3& pos, U32 tileIndex);
		void RemoveGround(const I32x3& pos);
		GameMapGrounds* GetGameMapGround();

		// serialize methods
		void LoadMapFromMapID(U32 mapID);
		void LoadMapFromFullPath(const std::string& fullPath);
		void LoadMapFromParentPath(const std::string& parentPath);
		void SaveByParentPath(const std::string& parentPath);

	private:
		U32 GenerateMapID();
		U32 GetMapIDFromFullPath(const std::string& fullPath)const;
		std::string GetMapFullPathFromID(const std::string& filePath, U32 mapID)const;
		std::string GetRealFileMapNameFromID(U32 mapID)const;

		GameMapPartPtr LoadMapPart(const std::string& parentPath, const MapPartPosition& mapPartPos);
		void SaveMapPart(const std::string& parentPath, GameMapPart& part);

	private:
		bool mIsLoaded = false;

		// map properties
		U32 mMapID = 0;
		I32 mMapWidth = 0;
		I32 mMapHeight = 0;
		I32 mMapLayer = 0;
		std::string mMapName = "";
		std::string mMapParentPath = "";

		MapTileset mMapTilset;
		std::vector<GameMapPartPtr> mGameMapParts;
	};
}