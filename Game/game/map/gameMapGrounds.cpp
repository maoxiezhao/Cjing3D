#include "gameMapGrounds.h"
#include "system\sceneSystem.h"
#include "game\map\gameMap.h"

namespace CjingGame
{
	namespace {
		ECS::Entity mMeshEntity = INVALID_ENTITY;
	}

	GameMapGrounds::GameMapGrounds(GameMap& gameMap) :
		mGameMap(gameMap)
	{
	}

	GameMapGrounds::~GameMapGrounds()
	{
	}

	void GameMapGrounds::Initialize()
	{
		mGroundTileset.LoadTilesetImage("Textures/GroundTilesets/beach.png");
		mGroundRenderer.Initialize();
		mGroundRenderer.SetCurrentTileset(&mGroundTileset);

	}

	void GameMapGrounds::FixedUpdate()
	{
	}

	void GameMapGrounds::Uninitialize()
	{
		for (auto& kvp : mAllGrounds) {
			kvp.second->RemoveFromScene();
		}
		mAllGrounds.clear();

		mGroundRenderer.Uninitilize();
	}

	void GameMapGrounds::PreRender()
	{
		mGroundRenderer.BindPreRender();
	}

	void GameMapGrounds::AddGround(const I32x3& pos)
	{
		// check is same ground?
		auto it = mAllGrounds.find(pos);
		if (it != mAllGrounds.end()) {
			return;
		}

		auto ground = mGroundRenderer.CreateGroundObject(Scene::GetScene());
		F32x3 globalPos = mGameMap.TransformLocalPosToGlobal(pos);
		ground->SetPosition(globalPos);

		mAllGrounds[pos] = ground;
	}

	void GameMapGrounds::RemoveGround(const I32x3& pos)
	{
		auto it = mAllGrounds.find(pos);
		if (it == mAllGrounds.end()) {
			return;
		}

		auto gameGround = it->second;
		gameGround->RemoveFromScene();
		mAllGrounds.erase(it);
	}
}