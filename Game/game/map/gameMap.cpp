#include "gameMap.h"
#include "gameMapRenderer.h"

#include <stack>

#define GAME_MAP_DEBUG

namespace CjingGame
{
	I32 MapPartPosition::GetLocalPosX() const
	{
		return x * GameContext::GameMapPartSize;
	}

	I32 MapPartPosition::GetLocalPosY() const
	{
		return y * GameContext::GameMapPartSize;
	}

	I32 MapPartPosition::GetLocalPosZ() const
	{
		return z * GameContext::GameMapPartSize;
	}

	MapPartPosition MapPartPosition::operator+(I32 v) const
	{
		return MapPartPosition(
			x + v,
			y + v,
			z + v
		);
	}

	MapPartPosition MapPartPosition::operator-(I32 v) const
	{
		return MapPartPosition(
			x - v,
			y - v,
			z - v
		);
	}

	MapPartPosition MapPartPosition::operator+(const MapPartPosition& v) const
	{
		return MapPartPosition(
			x + v.x,
			y + v.y,
			z + v.z
		);
	}

	MapPartPosition MapPartPosition::operator-(const MapPartPosition& v) const
	{
		return MapPartPosition(
			x - v.x,
			y - v.y,
			z - v.z
		);
	}

	bool MapPartPosition::operator!=(const MapPartPosition& rhs) const
	{
		return x != rhs.x || y != rhs.y || z != rhs.z;
	}

	bool MapPartPosition::operator==(const MapPartPosition& rhs) const
	{
		return x == rhs.x && y == rhs.y && z == rhs.z;
	}

	MapPartPosition MapPartPosition::TransformFromLocalPos(const I32x3& localPos)
	{
		return MapPartPosition(
			localPos[0] / GameContext::GameMapPartSize,
			localPos[1] / GameContext::GameMapPartSize,
			localPos[2] / GameContext::GameMapPartSize
		);
	}

	MapPartPosition MapPartPosition::TransformFromLocalPos(const I32x3& localPos, const MapPartPosition& currentPartPos)
	{
		return MapPartPosition(
			localPos[0] / GameContext::GameMapPartSize - currentPartPos.x,
			localPos[1] / GameContext::GameMapPartSize - currentPartPos.y,
			localPos[2] / GameContext::GameMapPartSize - currentPartPos.z
		);
	}

	GameMapPart::GameMapPart(GameMap& gameMap, const MapPartPosition& position) :
		mGameMap(gameMap),
		mPartPosition(position)
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
		mIsVisible = isVisible;

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

	GameMapGround* GameMapPart::RaycastMapGround(const Ray& ray, Scene::PickResult& pickResult)
	{
		return mGameMapGrounds->Raycast(ray, pickResult);
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
		mMapTilset.Bind();

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

		// update map parts
		for (auto& mapPart : mGameMapParts)
		{
			if (mapPart != nullptr && mapPart->IsVisible()) {
				mapPart->FixedUpdate();
			}
		}

		// update moving map parts
		if (mIsMapPartsDirty) 
		{
			mIsMapPartsDirty = false;
			RefreshMapParts();
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
		mGameMapParts.resize((size_t)mapPartSize * (size_t)mapPartSize * (size_t)mapPartSize, nullptr);

		mTotalMapPartSize = {
			(mMapWidth - 1  + GameContext::GameMapPartSize) / GameContext::GameMapPartSize,
			(mMapLayer - 1  + GameContext::GameMapPartSize) / GameContext::GameMapPartSize,
			(mMapHeight - 1 + GameContext::GameMapPartSize) / GameContext::GameMapPartSize,
		};

		// 根据当前位置加载范围内的MapParts
		I32 range = GameContext::GameMapPartRange;
		for (int y = -range; y <= range; y++)
		{
			for (int z = -range; z <= range; z++)
			{
				for (int x = -range; x <= range; x++)
				{
					MapPartPosition globalPos(
						x + defaultPartPos.x,
						y + defaultPartPos.y,
						z + defaultPartPos.z
					);
					if (globalPos.x < 0 || globalPos.x >= mTotalMapPartSize.x() ||
						globalPos.y < 0 || globalPos.y >= mTotalMapPartSize.y() ||
						globalPos.z < 0 || globalPos.z >= mTotalMapPartSize.z()) {
						continue;
					}
					LoadMapPart(globalPos, MapPartPosition(x, y, z));
				}
			}
		}
	}

	void GameMap::LoadMapPart(const MapPartPosition& globalPartPos, const MapPartPosition& localPartPos)
	{
#ifdef GAME_MAP_DEBUG
		Logger::InfoEx("LoadMapPart, global pos:%d,%d,%d, local pos:%d,%d,%d", globalPartPos.x, globalPartPos.y, globalPartPos.z, localPartPos.x, localPartPos.y, localPartPos.z);
#endif
		auto gameMapPart = LoadMapPart(mMapParentPath, globalPartPos);
		if (gameMapPart != nullptr) {
			gameMapPart->SetVisible(true);
		}

		SetMapPartByLocalPartPos(localPartPos, gameMapPart);
	}

	void GameMap::RefreshMapParts()
	{
		if (mNewPartPos == mCurrentPartPos) {
			return;
		}

#ifdef GAME_MAP_DEBUG
		Logger::InfoEx("RefreshMapParts");
#endif

		// 如果新的partPos和当前part距离小于partSize
		// 则动态刷新当前LoadedParts，将超出范围的part unload
		// load新加入范围的parts
		I32 partSize = GetMapPartSize();
		auto dif = mNewPartPos - mCurrentPartPos;
		if (std::abs(dif.x) < partSize &&
			std::abs(dif.y) < partSize &&
			std::abs(dif.z) < partSize)
		{
			DynamicRefreshMapParts(dif);
		}
		// 如果大于partRange，则在新的位置重新按range加载
		else
		{
			LoadMapParts(mNewPartPos);
		}

		mCurrentPartPos = mNewPartPos;
	}

	void GameMap::RemoveMapPartByLocalPartPos(const MapPartPosition& partPos)
	{
#ifdef GAME_MAP_DEBUG
		MapPartPosition globalPos = partPos + mCurrentPartPos;
		if (!(globalPos.x < 0 || globalPos.x >= mTotalMapPartSize.x() ||
			globalPos.y < 0 || globalPos.y >= mTotalMapPartSize.y() ||
			globalPos.z < 0 || globalPos.z >= mTotalMapPartSize.z())) 
		{
			Logger::InfoEx("RemoveMapPartByLocalPartPos, local pos:%d,%d,%d", partPos.x, partPos.y, partPos.z);
		}
#endif

		U32 index = GetMapPartIndexByLocalPartPos(partPos);
		if (index >= mGameMapParts.size()) {
			return;
		}

		auto mapPart = mGameMapParts[index];
		if (mapPart != nullptr)
		{
			mapPart->Uninitialize();
			mGameMapParts[index] = nullptr;
		}
	}

	void GameMap::MoveMapPartsByLocalPartPos(const MapPartPosition& partPos, const MapPartPosition& newPartPos)
	{
		if (newPartPos == partPos) {
			return;
		}

		U32 index = GetMapPartIndexByLocalPartPos(partPos);
		if (index >= mGameMapParts.size()) {
			return;
		}

		auto mapPart = mGameMapParts[index];
		if (mapPart != nullptr) {
			mGameMapParts[index] = nullptr;
		}

		SetMapPartByLocalPartPos(newPartPos, mapPart);
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

	GameMapPart* GameMap::GetMapPartByLocalPos(const I32x3& localPos)
	{
		MapPartPosition partPos = MapPartPosition::TransformFromLocalPos(localPos, mCurrentPartPos);
		U32 index = GetMapPartIndexByLocalPartPos(partPos);
		if (index >= mGameMapParts.size()) {
			return nullptr;
		}

		return mGameMapParts[index].get();
	}

	GameMapPart* GameMap::GetMapPartByLocalPartPos(const MapPartPosition& pos)
	{
		U32 index = GetMapPartIndexByLocalPartPos(pos);
		if (index >= mGameMapParts.size()) {
			return nullptr;
		}

		return mGameMapParts[index].get();
	}

	GameMapPart* GameMap::GetMapPartByGlobalPartPos(const MapPartPosition& pos)
	{
		U32 index = GetMapPartIndexByLocalPartPos(pos - mCurrentPartPos);
		if (index >= mGameMapParts.size()) {
			return nullptr;
		}

		return mGameMapParts[index].get();
	}

	MapPartPosition GameMap::TransformGlobalPartToLocal(const MapPartPosition& pos)const
	{
		return pos - mCurrentPartPos;
	}

	MapPartPosition GameMap::TransformLocalPartToGlobal(const MapPartPosition& pos)const
	{
		return pos + mCurrentPartPos;
	}

	void GameMap::SetMapPartByLocalPartPos(const MapPartPosition& partPos, GameMapPartPtr mapPart)
	{
		U32 index = GetMapPartIndexByLocalPartPos(partPos);
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

	GameMapPart* GameMap::GetMapPartByGlobalPos(const F32x3& globalPos)
	{
		return GetMapPartByLocalPos(TransformGlobalPosToLocal(globalPos));
	}

	void GameMap::AddGround(const I32x3& pos, U32 tileIndex)
	{
		GameMapPart* currentPart = GetMapPartByLocalPos(pos);
		if (currentPart != nullptr) {
			currentPart->AddGround(pos, tileIndex);
		}
	}

	void GameMap::RemoveGround(const I32x3& pos)
	{
		GameMapPart* currentPart = GetMapPartByLocalPos(pos);
		if (currentPart != nullptr) {
			currentPart->RemoveGround(pos);
		}
	}

	AABB GameMap::GetMapPartsBoundingBox() const
	{
		I32 range = GetMapPartRange();
		MapPartPosition leftBottomPos = mCurrentPartPos - range;
		MapPartPosition rightTopPos = mCurrentPartPos + range + 1;

		F32x3 minPos = F32x3(
			(F32)(leftBottomPos.GetLocalPosX() * GameContext::MapCellSize),
			(F32)(leftBottomPos.GetLocalPosY() * GameContext::MapCellSize),
			(F32)(leftBottomPos.GetLocalPosZ() * GameContext::MapCellSize)
		);
		F32x3 maxPos = F32x3(
			(F32)(rightTopPos.GetLocalPosX() * GameContext::MapCellSize),
			(F32)(rightTopPos.GetLocalPosY() * GameContext::MapCellSize),
			(F32)(rightTopPos.GetLocalPosZ() * GameContext::MapCellSize)
		);

		return AABB(minPos, maxPos);
	}

	U32 GameMap::GetMapPartIndexByLocalPartPos(const MapPartPosition& localPos) const
	{
		I32 range = GetMapPartRange();
		U32 mapPartSize = GetMapPartSize();
		return (localPos.y + range) * mapPartSize * mapPartSize +
			   (localPos.z + range) * mapPartSize + 
			   (localPos.x + range);
	}

	I32 GameMap::GetMapPartSize() const
	{
		return GameContext::GameMapPartRange * 2 + 1;
	}

	I32 GameMap::GetMapPartRange() const
	{
		return GameContext::GameMapPartRange;
	}

	void GameMap::SetCurrentPartPos(const MapPartPosition& pos)
	{
		if (pos != mCurrentPartPos)
		{
			mNewPartPos = pos;
			mIsMapPartsDirty = true;
		}
	}

	std::vector<MapPartPosition> GameMap::GetMapPartsPosByRay(const Ray& ray)
	{
		I32x3 localOriginPos = TransformGlobalPosToLocal(XMConvert(ray.mOrigin));
		MapPartPosition localOriginPartPos = GetLocalMapPartPosition(localOriginPos);

		// 获取摄像穿过的所有part
		// 如果相机在Parts里面则直接从射线起点part开始
		// 反之则先获取到相机射线和Parts最近的交点part
		std::vector<MapPartPosition> mapParts;
		if (IsLocalMapPartLoaded(localOriginPartPos)) {
			mapParts.push_back(localOriginPartPos);
		}
		else
		{
			AABB partsBoundingBox = GetMapPartsBoundingBox();
			F32 t = 0.0f;
			if (!ray.Intersects(partsBoundingBox, &t)) {
				return mapParts;
			}
			else
			{
				XMVECTOR hitPos = XMLoadFloat3(&ray.mOrigin) + XMLoadFloat3(&ray.mDirection) * t;
				I32x3 localPos = TransformGlobalPosToLocal(XMStore<F32x3>(hitPos));
				mapParts.push_back(GetLocalMapPartPosition(localPos));
			}
		}

		std::list <MapPartPosition> adjacents;
		if (ray.mDirection.x > 0)
			adjacents.push_back({ 1, 0, 0 });
		else if (ray.mDirection.x < 0)
			adjacents.push_back({ -1, 0, 0 });

		if (ray.mDirection.y > 0)
			adjacents.push_back({ 0, 1, 0 });
		else if (ray.mDirection.y < 0)
			adjacents.push_back({ 0, -1, 0 });

		if (ray.mDirection.z > 0)
			adjacents.push_back({ 0, 0, 1 });
		else if (ray.mDirection.z < 0)
			adjacents.push_back({ 0, 0, -1 });

		std::stack<MapPartPosition> testParts;
		testParts.push(mapParts.front());

		// 通过相邻Part检测来减少AABB射线相交计算次数
		while (!testParts.empty())
		{
			MapPartPosition partPos = testParts.top();
			testParts.pop();

			for (auto& adjacent : adjacents)
			{
				MapPartPosition localMapPart = partPos + adjacent;
				if (!IsLocalMapPartLoaded(localMapPart)) {
					continue;
				}

				MapPartPosition leftBottomPos = mCurrentPartPos + localMapPart;
				MapPartPosition rightTopPos = mCurrentPartPos + localMapPart + 1;

				F32x3 minPos = F32x3(
					(F32)(leftBottomPos.GetLocalPosX() * GameContext::MapCellSize),
					(F32)(leftBottomPos.GetLocalPosY() * GameContext::MapCellSize),
					(F32)(leftBottomPos.GetLocalPosZ() * GameContext::MapCellSize)
				);
				F32x3 maxPos = F32x3(
					(F32)(rightTopPos.GetLocalPosX() * GameContext::MapCellSize),
					(F32)(rightTopPos.GetLocalPosY() * GameContext::MapCellSize),
					(F32)(rightTopPos.GetLocalPosZ() * GameContext::MapCellSize)
				);

				if (ray.Intersects(AABB(minPos, maxPos)))
				{
					mapParts.push_back(localMapPart);
					testParts.push(localMapPart);
				}
			}
		}

		return mapParts;
	}

	std::vector<GameMapPart*> GameMap::GetMapPartsByRay(const Ray& ray)
	{
		std::vector<GameMapPart*> ret;
		for (const auto& partPos : GetMapPartsPosByRay(ray))
		{
			auto mapPart = GetMapPartByLocalPartPos(partPos);
			if (mapPart != nullptr) {
				ret.push_back(mapPart);
			}
		}
		return ret;
	}

	bool GameMap::IsLocalMapPartValid(const MapPartPosition& pos) const
	{
		auto globalPos = mCurrentPartPos + pos;
		return (globalPos.x >= 0 && globalPos.x < mTotalMapPartSize.x() &&
				globalPos.y >= 0 && globalPos.y < mTotalMapPartSize.y() &&
				globalPos.z >= 0 && globalPos.z < mTotalMapPartSize.z());
	}

	bool GameMap::IsLocalMapPartLoaded(const MapPartPosition& pos) const
	{
		I32 range = GetMapPartRange();
		return  IsLocalMapPartValid(pos) &&
				(pos.x >= -range && pos.x <= range) &&
				(pos.y >= -range && pos.y <= range) &&
				(pos.z >= -range && pos.z <= range);
	}

	MapPartPosition GameMap::GetLocalMapPartPosition(const I32x3& localPos)const
	{
		return MapPartPosition::TransformFromLocalPos(localPos, mCurrentPartPos);
	}

	MapPartPosition GameMap::GetGlobalMapPartPosition(const I32x3& localPos)const
	{
		return MapPartPosition::TransformFromLocalPos(localPos);
	}

	GameMap::GameMapPartPtr GameMap::LoadMapPart(const std::string& parentPath, const MapPartPosition& mapPartPos)
	{
		auto newMapPart = std::make_shared<GameMapPart>(*this, mapPartPos);
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

	void GameMap::DynamicRefreshMapParts(const MapPartPosition& dif)
	{
		if (dif.x != 0)
		{
			DynamicRefreshMapPartsImpl(dif.x, [](I32 x, I32 y, I32 z) {
				return MapPartPosition(x, y, z);
			});
			mCurrentPartPos.x = mCurrentPartPos.x + dif.x;
		}

		if (dif.y != 0)
		{
			DynamicRefreshMapPartsImpl(dif.y, [](I32 x, I32 y, I32 z) {
				return MapPartPosition(y, x, z);
				});
			mCurrentPartPos.y = mCurrentPartPos.y + dif.y;
		}

		if (dif.z != 0)
		{
			DynamicRefreshMapPartsImpl(dif.z, [](I32 x, I32 y, I32 z) {
				return MapPartPosition(y, z, x);
				});
			mCurrentPartPos.z = mCurrentPartPos.z + dif.z;
		}
	}

	void GameMap::DynamicRefreshMapPartsImpl(I32 dif, MapPartPosFunc partPosFunc)
	{
		I32 partSize = GetMapPartSize();
		I32 range = GetMapPartRange();
		I32 delta = abs(dif);

		if (dif > 0)
		{
			for (int y = -range; y <= range; y++)
			{
				for (int z = -range; z <= range; z++)
				{
					// remove parts
					for (int index = 0; index < delta; index++) {
						RemoveMapPartByLocalPartPos(partPosFunc(-range + index, y, z));
					}
					// move parts
					for (int index = 0; index < (partSize - delta); index++) {
						MoveMapPartsByLocalPartPos(partPosFunc(-range + index + delta, y, z), partPosFunc(-range + index, y, z));
					}
					// load parts
					for (int index = 1; index <= delta; index++)
					{
						MapPartPosition globalPos(
							mCurrentPartPos.x + range + index,
							mCurrentPartPos.y + y,
							mCurrentPartPos.z + z
						);
						if (globalPos.x < 0 || globalPos.x >= mTotalMapPartSize.x() ||
							globalPos.y < 0 || globalPos.y >= mTotalMapPartSize.y() ||
							globalPos.z < 0 || globalPos.z >= mTotalMapPartSize.z()) {
							continue;
						}
						LoadMapPart(globalPos, partPosFunc(range - (delta - index), y, z));
					}
				}
			}
		}
		else
		{
			for (int y = -range; y <= range; y++)
			{
				for (int z = -range; z <= range; z++)
				{
					// remove parts
					for (int index = 0; index < delta; index++) {
						RemoveMapPartByLocalPartPos(partPosFunc(range - index, y, z));
					}
					// move parts
					for (int index = 0; index < (partSize - delta); index++) {
						MoveMapPartsByLocalPartPos(partPosFunc(range - index - delta, y, z), partPosFunc(range - index, y, z));
					}
					// load parts
					for (int index = 1; index <= delta; index++)
					{
						MapPartPosition globalPos(
							mCurrentPartPos.x - range - index,
							mCurrentPartPos.y + y,
							mCurrentPartPos.z + z
						);
						if (globalPos.x < 0 || globalPos.x >= mTotalMapPartSize.x() ||
							globalPos.y < 0 || globalPos.y >= mTotalMapPartSize.y() ||
							globalPos.z < 0 || globalPos.z >= mTotalMapPartSize.z()) {
							continue;
						}
						LoadMapPart(globalPos, partPosFunc(-range + (delta - index), y, z));
					}
				}
			}
		}
	}
}