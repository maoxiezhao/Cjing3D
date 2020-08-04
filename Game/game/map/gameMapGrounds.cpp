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
		mInstance = GameMapGroundRenderer::CreateStaticInst();
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

		if (mInstance != nullptr) {
			GameMapGroundRenderer::RemoveStaticInst(mInstance);
		}
	}

	void GameMapGrounds::PreRender()
	{
		if (mIsGroundsDirty)
		{
			mIsGroundsDirty = false;
			GameMapGroundRenderer::GenerateMesh(mInstance->mMeshEntity, mAllStaticGrounds);
		}
	}

	void GameMapGrounds::SetVisible(bool isVisible)
	{
		if (isVisible != mIsVisible)
		{
			mIsVisible = isVisible;

			if (mInstance != nullptr && mInstance->mObjectEntity != INVALID_ENTITY) 
			{
				auto object = Scene::GetScene().mObjects.GetComponent(mInstance->mObjectEntity);
				if (object != nullptr) {
					object->SetRenderable(isVisible);
				}
			}
		}
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

	GameMapGround* GameMapGrounds::Raycast(const Ray& ray, Scene::PickResult& pickResult)
	{
		GameMapGround* ret = nullptr;

		XMVECTOR rayOrigin = XMLoadFloat3(&ray.mOrigin);
		XMVECTOR rayDir = XMLoadFloat3(&ray.mDirection);

		I32 mapCellSize = GameContext::MapCellSize;
		F32x3 mapCellOffset = F32x3((F32)mapCellSize, 0.0f, (F32)mapCellSize);
		for (const auto& kvp : mAllStaticGrounds)
		{

			I32x3 localPos = kvp.first;
			F32x3 globalPos = {
				(F32)(localPos[0] * mapCellSize),
				(F32)(localPos[1] * mapCellSize),
				(F32)(localPos[2] * mapCellSize)
			};
			AABB groundAABB(
				globalPos,
				globalPos + mapCellOffset
			);

			F32 currentT = 0.0f;
			if (!ray.Intersects(groundAABB, &currentT)) {
				continue;
			}
		
			XMVECTOR hitPos = rayOrigin + rayDir * currentT;
			F32 distance = XMDistance(rayOrigin, hitPos);
			if (distance < pickResult.distance)
			{
				pickResult.distance = distance;
				pickResult.position = XMStore<F32x3>(hitPos);
				pickResult.entity = mInstance->mObjectEntity;

				ret = kvp.second.get();
			}
		}

		return ret;
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