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
		mMapPath(fullPath)
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
		mGameMapPart = std::make_unique<GameMapPart>(*this);
		mGameMapPart->Initialize();

		mIsLoaded = true;
	}

	void GameMap::LoadMapFromMapID(U32 mapID)
	{
		if (mIsLoaded) {
			Clear();
		}

		// get map path from config
		const std::string mapFilePath = "";
		mMapPath = GetMapFullPathFromID(mapFilePath, mapID);
		LoadMapFromFullPath(mMapPath);
	}

	void GameMap::FixedUpdate()
	{
		if (!mIsLoaded) {
			return;
		}
	}

	void GameMap::Clear()
	{
		if (!mIsLoaded) {
			return;
		}

		if (mGameMapPart != nullptr)
		{
			mGameMapPart->Uninitialize();
			mGameMapPart = nullptr;
		}

		mIsLoaded = false;
	}

	void GameMap::PreRender()
	{
		if (!mIsLoaded) {
			return;
		}

		mGameMapPart->PreRender();
	}

	I32x3 GameMap::TransformGlobalPosToLocal(const F32x3& pos) const
	{
		return I32x3(
			std::max(0, (I32)std::floor(pos.x() / MapCellSize)),
			std::max(0, (I32)std::floor(pos.y() / MapCellSize)),
			std::max(0, (I32)std::floor(pos.z() / MapCellSize))
		);
	}

	F32x3 GameMap::TransformLocalPosToGlobal(const I32x3& pos) const
	{
		return F32x3(
			(F32)pos.x() * MapCellSize,
			(F32)pos.y() * MapCellSize,
			(F32)pos.z() * MapCellSize
		);
	}

	GameMapPart* GameMap::GetCurrentMapPartByLocalPos(const I32x3& localPos)
	{
		return mGameMapPart.get();
	}

	GameMapPart* GameMap::GetCurrentMapPartByGlobalPos(const F32x3& globalPos)
	{
		return mGameMapPart.get();
	}

	void GameMap::AddGround(const I32x3& pos, U32 tileIndex)
	{
		mGameMapPart->AddGround(pos, tileIndex);
	}

	void GameMap::RemoveGround(const I32x3& pos)
	{
		mGameMapPart->RemoveGround(pos);
	}

	GameMapGrounds* GameMap::GetGameMapGround()
	{
		return mGameMapPart != nullptr ? mGameMapPart->GetGameMapGrounds() : nullptr;
	}

	void GameMap::LoadMapPart(const std::string& parentPath, const I32x3& localPos)
	{
		const std::string partName = "part.json";
		const std::string mapInfoPath = FileData::CombinePath(parentPath, partName);

		JsonArchive archive(mapInfoPath, ArchiveMode::ArchiveMode_Read);
		if (!archive.IsOpen()) {
			return;
		}

		GameMapPart* gameMapPart = GetCurrentMapPartByLocalPos(localPos);
		if (gameMapPart == nullptr) 
		{
			mGameMapPart = std::make_unique<GameMapPart>(*this);
			mGameMapPart->Initialize();
		}

		mGameMapPart->Serialize(archive);
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
		// map info
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
		}

		// load parts
		LoadMapPart(fullPath, { 0, 0, 0 });

		mIsLoaded = true;
	}


	void GameMap::LoadFromFile(const std::string& filePath)
	{
		std::string fullPath = GetMapFullPathFromID(filePath, mMapID);
		LoadMapFromFullPath(fullPath);
	}

	void GameMap::SaveToFile(const std::string& filePath)
	{
		std::string realMapName = GetRealFileMapNameFromID(mMapID);
		const std::string fullFilePath = FileData::CombinePath(filePath, realMapName);
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
		SaveMapPart(fullFilePath , *mGameMapPart);	
	}

	void GameMap::SaveMapPart(const std::string& parentPath, GameMapPart& part)
	{
		const std::string partName = "part.json";
		const std::string mapInfoPath = FileData::CombinePath(parentPath, partName);
		JsonArchive archive(mapInfoPath, ArchiveMode::ArchiveMode_Write);
		part.Unserialize(archive);
	}

}