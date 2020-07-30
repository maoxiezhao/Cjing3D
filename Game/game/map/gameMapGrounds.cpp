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
		for (auto& kvp : mAllDynamicGrounds) {
			kvp.second->RemoveFromScene();
		}
		mAllDynamicGrounds.clear();
		mAllStaticGrounds.clear();
		mGroundRenderer.Uninitilize();
	}

	void GameMapGrounds::PreRender()
	{
		if (mIsGroundsDirty)
		{
			mIsGroundsDirty = false;
			mGroundRenderer.GenerateMesh(mAllStaticGrounds);
		}

		mGroundRenderer.BindPreRender();
	}

	void GameMapGrounds::AddGround(const I32x3& pos, U32 tileIndex)
	{
		auto it = mAllStaticGrounds.find(pos);
		if (it != mAllStaticGrounds.end()) 
		{
			auto ground = it->second;
			if (ground->GetTileIndex() != tileIndex) 
			{
				ground->SetTileIndex(tileIndex);
				mIsGroundsDirty = true;
			}
			return;
		}

		auto ground = std::make_shared<GameMapGround>();
		ground->SetLocalPos(pos);
		ground->SetTileIndex(tileIndex);

		mAllStaticGrounds[pos] = ground;
		mIsGroundsDirty = true;
	}

	void GameMapGrounds::RemoveGround(const I32x3& pos)
	{
		auto it = mAllStaticGrounds.find(pos);
		if (it == mAllStaticGrounds.end()) {
			return;
		}

		auto gameGround = it->second;
		mAllStaticGrounds.erase(it);
		mIsGroundsDirty = true;
	}

	void GameMapGrounds::Serialize(JsonArchive& archive)
	{
		size_t count = archive.GetCurrentValueCount();
		for (size_t index = 0; index < count; index++)
		{
			archive.Read(index, [this](JsonArchive& archive) 
			{
				I32x3 pos = I32x3(0, 0, 0);
				archive.Read("pos", pos);

				auto ground = std::make_shared<GameMapGround>();
				ground->SetLocalPos(pos);
				archive.Read("val", *ground);

				mAllStaticGrounds[pos] = ground;
			});
		}
		mIsGroundsDirty = count > 0;
	}

	void GameMapGrounds::Unserialize(JsonArchive& archive)const
	{
		for (const auto& kvp : mAllStaticGrounds)
		{
			archive.PushArray([&kvp](JsonArchive& archive) 
			{
				archive.Write("pos", kvp.first);
				archive.Write("val", *kvp.second);
				archive.Pop();
			});
		}
	}
}