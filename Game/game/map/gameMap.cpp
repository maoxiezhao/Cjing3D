#include "gameMap.h"

namespace CjingGame
{

	GameMapObjects::GameMapObjects(GameMap& gameMap) :
		mGameMap(gameMap)
	{
	}

	GameMapObjects::~GameMapObjects()
	{
	}

	void GameMapObjects::Initialize()
	{
		mGameMapGrounds = std::make_unique<GameMapGrounds>(mGameMap);
		mGameMapGrounds->Initialize();
	}

	void GameMapObjects::FixedUpdate()
	{
		mGameMapGrounds->FixedUpdate();
	}

	void GameMapObjects::Uninitialize()
	{
		mGameMapGrounds->Uninitialize();
		mGameMapGrounds = nullptr;
	}

	void GameMapObjects::PreRender()
	{
		mGameMapGrounds->PreRender();
	}

	bool GameMapObjects::AddGround(const I32x3& localPos)
	{
		mGameMapGrounds->AddGround(localPos);
		return true;
	}

	bool GameMapObjects::RemoveGround(const I32x3& localPos)
	{
		mGameMapGrounds->RemoveGround(localPos);
		return true;
	}

	////////////////////////////////////////////////////////////////////////////////////

	GameMap::GameMap()
	{
	}

	GameMap::~GameMap()
	{
	}

	void GameMap::Initialize(I32 width, I32 height, I32 layer)
	{
		if (mIsInitialized) {
			return;
		}

		mMapWidth = width;
		mMapHeight = height;
		mMapLayer = layer;

		mGameMapObjects = std::make_unique<GameMapObjects>(*this);
		mGameMapObjects->Initialize();

		mIsInitialized = true;
	}

	void GameMap::Initialize(const std::string& filePath)
	{
		if (mIsInitialized) {
			return;
		}

		mGameMapObjects = std::make_unique<GameMapObjects>(*this);
		mGameMapObjects->Initialize();

		mIsInitialized = true;
	}

	void GameMap::FixedUpdate()
	{
		if (!mIsInitialized) {
			return;
		}
	}

	void GameMap::Uninitialize()
	{
		if (!mIsInitialized) {
			return;
		}

		if (mGameMapObjects != nullptr)
		{
			mGameMapObjects->Uninitialize();
			mGameMapObjects = nullptr;
		}

		mIsInitialized = false;
	}

	void GameMap::PreRender()
	{
		if (!mIsInitialized) {
			return;
		}

		mGameMapObjects->PreRender();
	}

	void GameMap::LoadFromFile(const std::string& filePath)
	{
	}

	void GameMap::SaveToFile(const std::string& filePath)
	{
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
	void GameMap::AddGround(const I32x3& pos)
	{
		mGameMapObjects->AddGround(pos);
	}

	void GameMap::RemoveGround(const I32x3& pos)
	{
		mGameMapObjects->RemoveGround(pos);
	}
}