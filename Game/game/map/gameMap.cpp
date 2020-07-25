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
	}

	void GameMapObjects::FixedUpdate()
	{
	}

	void GameMapObjects::Uninitialize()
	{
	}

	bool GameMapObjects::AddGround(const I32x3& localPos)
	{
		return false;
	}

	bool GameMapObjects::RemoveGround(const I32x3& localPos)
	{
		return false;
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

		mGameMapObjects->Uninitialize();
		mGameMapObjects = nullptr;

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
		mIsInitialized = false;
	}

	void GameMap::LoadFromFile(const std::string& filePath)
	{
	}

	void GameMap::SaveToFile(const std::string& filePath)
	{
	}

	I32x3 GameMap::TransformGlobalPosToLocal(const F32x3& pos) const
	{
		return I32x3();
	}

	F32x3 GameMap::TransformLocalPosToGlobal(const I32x3& pos) const
	{
		return F32x3();
	}
}