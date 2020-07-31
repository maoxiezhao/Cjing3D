#include "gameMap.h"

namespace CjingGame
{

	GameMapPart::GameMapPart(GameMap& gameMap) :
		mGameMap(gameMap)
	{
	}

	GameMapPart::~GameMapPart()
	{
	}

	void GameMapPart::Initialize()
	{
		mGameMapGrounds = std::make_unique<GameMapGrounds>(mGameMap);
		mGameMapGrounds->Initialize();
	}

	void GameMapPart::FixedUpdate()
	{
		mGameMapGrounds->FixedUpdate();
	}

	void GameMapPart::Uninitialize()
	{
		mGameMapGrounds->Uninitialize();
		mGameMapGrounds = nullptr;
	}

	void GameMapPart::PreRender()
	{
		mGameMapGrounds->PreRender();
	}

	void GameMapPart::SetVisible(bool isVisible)
	{
		if (isVisible == mIsVisible) {
			return;
		}

	}

	bool GameMapPart::AddGround(const I32x3& localPos, U32 tileIndex)
	{
		mGameMapGrounds->AddGround(localPos, tileIndex);
		return true;
	}

	bool GameMapPart::RemoveGround(const I32x3& localPos)
	{
		mGameMapGrounds->RemoveGround(localPos);
		return true;
	}

	GameMapGrounds* GameMapPart::GetGameMapGrounds()
	{
		return mGameMapGrounds.get();
	}

	void GameMapPart::Serialize(JsonArchive& archive)
	{
		archive.Read("grounds", *mGameMapGrounds);
	}

	void GameMapPart::Unserialize(JsonArchive& archive)const
	{
		archive.Write("grounds", *mGameMapGrounds);
	}

	////////////////////////////////////////////////////////////////////////////////////

	GameMap::GameMap(I32 width, I32 height, I32 layer, const std::string& name) :
		mMapName(name),
		mMapID(GenerateMapID())
	{
		InitiEmptyMap(width, height, layer);
	}

	GameMap::GameMap(U32 mapID) :
		mMapID(mapID)
	{
		LoadMapFromMapID(mapID);
	}

	GameMap::GameMap(const std::string& fullPath) :
		mMapID(GetMapIDFromFullPath(fullPath)),
		mMapParentPath(fullPath)
	{
		LoadMapFromFullPath(fullPath);
	}

	GameMap::~GameMap()
	{
		Clear();
	}

	void GameMap::InitiEmptyMap(I32 width, I32 height, I32 layer)
	{
		if (mIsLoaded) 
		{
			Clear();
			return;
		}

		mMapWidth = width;
		mMapHeight = height;
		mMapLayer = layer;
		mMapTilset.mGroundTileset.LoadTilesetImage("Textures/GroundTilesets/beach.png");

		mIsLoaded = true;
	}

	void GameMap::LoadMapFromMapID(U32 mapID)
	{
		if (mIsLoaded) {
			Clear();
		}

		mMapParentPath = GetMapFullPathFromID(GameContext::GameMapCurrentParentPath, mapID);
		LoadMapFromFullPath(mMapParentPath);
	}

	void GameMap::FixedUpdate()
	{
		if (!mIsLoaded) {
			return;
		}

		for (auto& mapPart : mGameMapParts)
		{
			if (mapPart != nullptr && mapPart->IsVisible()) {
				mapPart->FixedUpdate();
			}
		}
	}

	void GameMap::Clear()
	{
		if (!mIsLoaded) {
			return;
		}

		ClearAllMapParts();

		mIsLoaded = false;
	}

	void GameMap::PreRender()
	{
		if (!mIsLoaded) {
			return;
		}

		for (auto& mapPart : mGameMapParts)
		{
			if (mapPart != nullptr && mapPart->IsVisible()) {
				mapPart->PreRender();
			}
		}
	}

	void GameMap::LoadMapParts(const MapPartPosition& defaultPartPos)
	{
		ClearAllMapParts();

		U32 mapPartSize = GetMapPartSize();
		mGameMapParts.resize(mapPartSize * mapPartSize * mapPartSize, nullptr);

		I32x3 totalPartCount = {
			(mMapWidth - 1 + GameContext::GameMapPartSize) / GameContext::GameMapPartSize,
			(mMapLayer - 1 + GameContext::GameMapPartSize) / GameContext::GameMapPartSize,
			(mMapHeight - 1 + GameContext::GameMapPartSize) / GameContext::GameMapPartSize,
		};

		I32 range = GameContext::GameMapPartRange;
		for (int y = -range; y <= range; y++)
		{
			for (int z = -range; z <= range; z++)
			{
				for (int x = -range; x <= range; x++)
				{
					MapPartPosition pos(
						x + defaultPartPos.x,
						y + defaultPartPos.y,
						z + defaultPartPos.z
					);
					if (pos.x < 0 || pos.x >= totalPartCount.x() ||
						pos.y < 0 || pos.y >= totalPartCount.y() ||
						pos.z < 0 || pos.z >= totalPartCount.z()) {
						continue;
					}
					LoadMapPart(pos);
				}
			}
		}
	}

	void GameMap::LoadMapPart(const MapPartPosition& mapPartPos)
	{
		auto gameMapPart = LoadMapPart(mMapParentPath, mapPartPos);
		if (gameMapPart != nullptr) {
			gameMapPart->SetVisible(true);
		}

		SetMapPartByPartPos(mapPartPos, gameMapPart);
	}

	void GameMap::RefreshMapParts(const MapPartPosition& localPos)
	{
	}

	I32x3 GameMap::TransformGlobalPosToLocal(const F32x3& pos) const
	{
		return I32x3(
			std::max(0, (I32)std::floor(pos.x() / GameContext::MapCellSize)),
			std::max(0, (I32)std::floor(pos.y() / GameContext::MapCellSize)),
			std::max(0, (I32)std::floor(pos.z() / GameContext::MapCellSize))
		);
	}

	F32x3 GameMap::TransformLocalPosToGlobal(const I32x3& pos) const
	{
		return F32x3(
			(F32)pos.x() * GameContext::MapCellSize,
			(F32)pos.y() * GameContext::MapCellSize,
			(F32)pos.z() * GameContext::MapCellSize
		);
	}

	GameMapPart* GameMap::GetCurrentMapPartByLocalPos(const I32x3& localPos)
	{
		MapPartPosition partPos = MapPartPosition::TransformFromLocalPos(localPos);
		U32 index = GetMapPartIndexByPartPos(partPos);
		if (index >= mGameMapParts.size()) {
			return nullptr;
		}

		return mGameMapParts[index].get();
	}

	void GameMap::SetMapPartByPartPos(const MapPartPosition& pos, GameMapPartPtr mapPart)
	{
		U32 index = GetMapPartIndexByPartPos(pos);
		if (index >= mGameMapParts.size())
		{
			Debug::Warning("[SetMapPartByLocalPos] Invalid map part index.");
			return;
		}

		mGameMapParts[index] = mapPart;
	}

	void GameMap::ClearAllMapParts()
	{
		for (auto& mapPart : mGameMapParts) 
		{
			if (mapPart != nullptr) {
				mapPart->Uninitialize();
			}
		}
		mGameMapParts.clear();
	}

	GameMapPart* GameMap::GetCurrentMapPartByGlobalPos(const F32x3& globalPos)
	{
		return GetCurrentMapPartByLocalPos(TransformGlobalPosToLocal(globalPos));
	}

	void GameMap::AddGround(const I32x3& pos, U32 tileIndex)
	{
		GameMapPart* currentPart = GetCurrentMapPartByLocalPos(pos);
		if (currentPart != nullptr) {
			currentPart->AddGround(pos, tileIndex);
		}
	}

	void GameMap::RemoveGround(const I32x3& pos)
	{
		GameMapPart* currentPart = GetCurrentMapPartByLocalPos(pos);
		if (currentPart != nullptr) {
			currentPart->RemoveGround(pos);
		}
	}

	GameMapGrounds* GameMap::GetGameMapGround()
	{
		return nullptr;
	}

	U32 GameMap::GetMapPartIndexByPartPos(const MapPartPosition& localPos) const
	{
		U32 mapPartSize = GetMapPartSize();
		return localPos.y * mapPartSize * mapPartSize + localPos.z * mapPartSize + localPos.x;
	}

	U32 GameMap::GetMapPartSize() const
	{
		return GameContext::GameMapPartRange * 2 + 1;
	}


	GameMap::GameMapPartPtr GameMap::LoadMapPart(const std::string& parentPath, const MapPartPosition& mapPartPos)
	{
		auto newMapPart = std::make_shared<GameMapPart>(*this);
		newMapPart->Initialize();

		char partName[64];
		sprintf(partName, "part_%d_%d_%d", mapPartPos.x, mapPartPos.y, mapPartPos.z);
		const std::string mapInfoPath = FileData::CombinePath(parentPath, partName);

		JsonArchive archive(mapInfoPath, ArchiveMode::ArchiveMode_Read);
		if (archive.IsOpen()) {
			newMapPart->Serialize(archive);
		}
		
		return newMapPart;
	}

	U32 GameMap::GenerateMapID()
	{
		return 1;
	}

	U32 GameMap::GetMapIDFromFullPath(const std::string& fullPath) const
	{
		return 1;
	}

	std::string GameMap::GetMapFullPathFromID(const std::string& filePath, U32 mapID) const
	{
		return FileData::CombinePath(filePath, GetRealFileMapNameFromID(mapID));
	}

	std::string GameMap::GetRealFileMapNameFromID(U32 mapID) const
	{
		return "Map" + std::to_string(mapID);
	}

	void GameMap::LoadMapFromFullPath(const std::string& fullPath)
	{
		const std::string mapInfoPath = FileData::CombinePath(fullPath, "info.json");
		JsonArchive archive(mapInfoPath, ArchiveMode::ArchiveMode_Read);
		if (!archive.IsOpen())
		{
			Debug::Warning("Failed to open map:" + fullPath);
			return;
		}

		archive.Read("name", mMapName);
		archive.Read("width", mMapWidth);
		archive.Read("height", mMapHeight);
		archive.Read("layer", mMapLayer);

		mIsLoaded = true;
	}

	void GameMap::LoadMapFromParentPath(const std::string& parentPath)
	{
		std::string fullPath = GetMapFullPathFromID(parentPath, mMapID);
		LoadMapFromFullPath(fullPath);
	}

	void GameMap::SaveByParentPath(const std::string& parentPath)
	{
		std::string realMapName = GetRealFileMapNameFromID(mMapID);
		const std::string fullFilePath = FileData::CombinePath(parentPath, realMapName);
		if (!FileData::CreateDirectory(fullFilePath))
		{
			Debug::Warning("Failed to create directory:" + fullFilePath);
			return;
		}
		// map info
		{
			const std::string mapInfoPath = FileData::CombinePath(fullFilePath, "info.json");
			JsonArchive archive(mapInfoPath, ArchiveMode::ArchiveMode_Write);
			archive.Write("name", mMapName);
			archive.Write("width", mMapWidth);
			archive.Write("height", mMapHeight);
			archive.Write("layer", mMapLayer);
		}

		// map parts
		for (auto& mapPart : mGameMapParts) {
			SaveMapPart(fullFilePath, *mapPart);
		}
	}

	void GameMap::SaveMapPart(const std::string& parentPath, GameMapPart& part)
	{
		const std::string partName = "part.json";
		const std::string mapInfoPath = FileData::CombinePath(parentPath, partName);
		JsonArchive archive(mapInfoPath, ArchiveMode::ArchiveMode_Write);
		part.Unserialize(archive);
	}

	MapPartPosition MapPartPosition::TransformFromLocalPos(const I32x3& localPos)
	{
		return MapPartPosition(
			localPos[0] / GameContext::GameMapPartSize,
			localPos[1] / GameContext::GameMapPartSize,
			localPos[2] / GameContext::GameMapPartSize
		);
	}
}