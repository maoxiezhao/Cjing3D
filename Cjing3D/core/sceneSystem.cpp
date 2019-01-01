#include "sceneSystem.h"

namespace Cjing3D
{
	using namespace ECS;

	Scene::Scene()
	{
	}

	void Scene::Update()
	{
	}

	void Scene::Merge(Scene & scene)
	{
		mNames.Merge(scene.mNames);
		mMeshes.Merge(scene.mMeshes);
		mMaterials.Merge(scene.mMaterials);

		// TEMP
		//mNameEntityMap.clear();
		//mNames.ForEachComponent([&](Entity enity, std::shared_ptr<StringID> stringID) {
		//	mNameEntityMap[*stringID] = enity;
		//});
	}

	void Scene::Clera()
	{
		mNameEntityMap.clear();
		mNames.Clear();
		mMaterials.Clear();
		mMeshes.Clear();
	}

	ECS::Entity Scene::CreateEntityMaterial(const std::string & name)
	{
		auto entity = CreateEntityByName(name);
		mMaterials.Create(entity);

		return entity;
	}

	ECS::Entity Scene::CreateEntityMesh(const std::string & name)
	{
		auto entity = CreateEntityByName(name);
		mMeshes.Create(entity);

		return entity;
	}

	ECS::Entity Scene::CreateEntityByName(const std::string & name)
	{
		// 目前不支持名字重名，后一个将无法添加
		auto it = mNameEntityMap.find(name);
		if (it != mNameEntityMap.end())
		{
			Debug::Warning("Duplicate entity name:" + name);
			return it->second;
		}

		Entity entity = CreateEntity();
		mNames.Create(entity)->SetString(name);
		mNameEntityMap[StringID(name)] = entity;

		return entity;
	}

	Entity Scene::GetEntityByName(StringID name)
	{
		auto it = mNameEntityMap.find(name);
		if (it != mNameEntityMap.end())
		{
			return it->second;
		}
		return ECS::INVALID_ENTITY;
	}

	void Scene::RemoveEntity(Entity entity)
	{
		mMaterials.Remove(entity);
		mMeshes.Remove(entity);

		if (mNames.Contains(entity))
		{
			auto nameComponent = mNames.GetComponent(entity);
			if (nameComponent != nullptr)
			{
				mNameEntityMap.erase(*nameComponent);
			}
			mNames.Remove(entity);
		}
	}
}