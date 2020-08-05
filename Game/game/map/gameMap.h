#pragma once

#include "game\map\mapInclude.h"
#include "game\map\ground\gameMapGrounds.h"
#include "game\map\mapTileset.h"

namespace CjingGame
{
	class GameMap;

	// Global Pos: world space pos
	// Local Pos: Game map grid pos
	// Map Part Pos: local map part pos based on current pos
	// Global map part pos: global map part pos based on original
	struct MapPartPosition
	{
	public:
		I32 x = 0;
		I32 y = 0;
		I32 z = 0;

	public:
		MapPartPosition() = default;
		MapPartPosition(I32 xx, I32 yy, I32 zz) : x(xx), y(yy), z(zz) {}

		I32 GetLocalPosX()const;
		I32 GetLocalPosY()const;
		I32 GetLocalPosZ()const;

		MapPartPosition operator+(I32 v)const;
		MapPartPosition operator-(I32 v)const;
		MapPartPosition operator+(const MapPartPosition& v)const;
		MapPartPosition operator-(const MapPartPosition& v)const;

		bool operator != (const MapPartPosition& rhs)const;
		bool operator == (const MapPartPosition& rhs)const;

		static MapPartPosition TransformFromLocalPos(const I32x3& localPos);
		static MapPartPosition TransformFromLocalPos(const I32x3& localPos, const MapPartPosition& currentPartPos);
	};

	class GameMapPart : public JsonSerializer
	{
	public:
		GameMapPart(GameMap& gameMap, const MapPartPosition& position);
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

		// raycast
		GameMapGround* RaycastMapGround(const Ray& ray, Scene::PickResult& pickResult);

		virtual void Serialize(JsonArchive& archive);
		virtual void Unserialize(JsonArchive& archive)const;

	private:
		GameMap& mGameMap;
		MapPartPosition mPartPosition;
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

		//////////////////////////////////////////////////////////////////////////////
		// system
		void InitiEmptyMap(I32 width, I32 height, I32 layer);
		void Clear();
		void FixedUpdate();
		void PreRender();

		I32x3 TransformGlobalPosToLocal(const F32x3& pos)const;
		F32x3 TransformLocalPosToGlobal(const I32x3& pos)const;

		//////////////////////////////////////////////////////////////////////////////
		// map parts
		void LoadMapParts(const MapPartPosition& defaultPartPos);
		void LoadMapPart(const MapPartPosition& globalPartPos, const MapPartPosition& localPartPos);
		void RefreshMapParts();
		void RemoveMapPartByLocalPartPos(const MapPartPosition& partPos);
		void MoveMapPartsByLocalPartPos(const MapPartPosition& partPos, const MapPartPosition& newPartPos);
		void ClearAllMapParts();
		void SetMapPartByLocalPartPos(const MapPartPosition& partPos, GameMapPartPtr mapPart);
		std::vector<MapPartPosition> GetMapPartsPosByRay(const Ray& ray);
		std::vector<GameMapPart*> GetMapPartsByRay(const Ray& ray);

		bool IsLocalMapPartValid(const MapPartPosition& pos)const;
		bool IsLocalMapPartLoaded(const MapPartPosition& pos)const;
		MapPartPosition GetLocalMapPartPosition(const I32x3& localPos)const;
		MapPartPosition GetGlobalMapPartPosition(const I32x3& localPos)const;

		GameMapPart* GetMapPartByGlobalPos(const F32x3& globalPos);
		GameMapPart* GetMapPartByLocalPos(const I32x3& localPos);
		GameMapPart* GetMapPartByLocalPartPos(const MapPartPosition& pos);
		GameMapPart* GetMapPartByGlobalPartPos(const MapPartPosition& pos);

		MapPartPosition TransformGlobalPartToLocal(const MapPartPosition& pos)const;
		MapPartPosition TransformLocalPartToGlobal(const MapPartPosition& pos)const;

		AABB GetMapPartsBoundingBox()const;
		U32 GetMapPartIndexByLocalPartPos(const MapPartPosition& localPos)const;
		I32 GetMapPartSize()const;
		I32 GetMapPartRange()const;
		void SetCurrentPartPos(const MapPartPosition& pos);

		//////////////////////////////////////////////////////////////////////////////
		// getter
		I32 GetMapWidth()const { return mMapWidth; }
		I32 GetMapHeight()const { return mMapHeight; }
		I32 GetMapLayer()const { return mMapLayer; }
		std::string GetMapName()const { return mMapName; }
		U32 GetMapID()const { return mMapID; }
		MapTileset& GetMapTileset() { return mMapTilset; }

		//////////////////////////////////////////////////////////////////////////////
		// setter
		void SetMapName(const std::string& name) { mMapName = name; }

		//////////////////////////////////////////////////////////////////////////////
		// ground
		void AddGround(const I32x3& pos, U32 tileIndex);
		void RemoveGround(const I32x3& pos);

		//////////////////////////////////////////////////////////////////////////////
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
		void DynamicRefreshMapParts(const MapPartPosition& dif);

		using MapPartPosFunc = std::function<MapPartPosition(I32, I32, I32)>;
		void DynamicRefreshMapPartsImpl(I32 dif, MapPartPosFunc partPosFunc);

	private:
		bool mIsLoaded = false;

		// map properties
		U32 mMapID = 0;
		I32 mMapWidth = 0;
		I32 mMapHeight = 0;
		I32 mMapLayer = 0;
		std::string mMapName = "";
		std::string mMapParentPath = "";

		I32x3 mTotalMapPartSize = I32x3(0, 0, 0);
		std::vector<GameMapPartPtr> mGameMapParts;
		MapPartPosition mCurrentPartPos;
		MapPartPosition mNewPartPos;
		bool mIsMapPartsDirty = false;

		MapTileset mMapTilset;
	};
}