#include "sceneSystem.h"

namespace Cjing3D
{
	using namespace ECS;

	Scene::Scene()
	{
	}

	void Scene::Update(F32 deltaTime)
	{
		UpdateSceneTransformSystem(*this);
		UpdateSceneLightSystem(*this);
		UpdateSceneObjectSystem(*this);
	}

	void Scene::Merge(Scene & scene)
	{
		mNames.Merge(scene.mNames);
		mMeshes.Merge(scene.mMeshes);
		mMaterials.Merge(scene.mMaterials);
		mObjects.Merge(scene.mObjects);
		mObjectAABBs.Merge(scene.mObjectAABBs);
		mTransforms.Merge(scene.mTransforms);
		mLightAABBs.Merge(scene.mLightAABBs);
		mLights.Merge(scene.mLights);
	}

	void Scene::Clear()
	{
		mNameEntityMap.clear();
		mNames.Clear();
		mMaterials.Clear();
		mMeshes.Clear();
		mObjects.Clear();
		mObjectAABBs.Clear();
		mTransforms.Clear();
		mLightAABBs.Clear();
		mLights.Clear();
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

	ECS::Entity Scene::CreateEntityObject(const std::string & name)
	{
		auto entity = CreateEntityByName(name);
		mObjects.Create(entity);
		mObjectAABBs.Create(entity);
		mTransforms.Create(entity);

		return entity;
	}

	ECS::Entity Scene::CreateEntityLight(const std::string & name, const F32x3 & pos, const F32x3 & color, F32 energy, F32 range)
	{
		auto entity = CreateEntityByName(name);
		TransformComponent& transform = *mTransforms.Create(entity);
		transform.Translate(XMConvert(pos));
		transform.Update();

		mLightAABBs.Create(entity);

		LightComponent& light = *mLights.Create(entity);
		light.mColor = color;
		light.mPosition = pos;
		light.mEnergy = energy;
		light.mRange = range;
		light.SetLightType(LightComponent::LightType_Point);

		return entity;
	}

	TransformComponent & Scene::GetOrCreateTransformByEntity(ECS::Entity entity)
	{
		auto transformPtr = mTransforms.GetComponent(entity);
		if (transformPtr != nullptr) {
			return *transformPtr;
		}

		TransformComponent& transform = *mTransforms.Create(entity);
		return transform;
	}

	// 仅创建light相关component，不包含transform
	LightComponent & Scene::GetOrCreateLightByEntity(ECS::Entity entity)
	{
		auto lightPtr = mLights.GetComponent(entity);
		if (lightPtr != nullptr) {
			return *lightPtr;
		}

		mLightAABBs.Create(entity);
		LightComponent& light = *mLights.Create(entity);
		light.SetLightType(LightComponent::LightType_Point);

		return light;
	}

	ECS::Entity Scene::CreateEntityByName(const std::string & name)
	{
		// 目前不支持名字重名，后一个将无法添加
		auto nameID = StringID(name);
		auto it = mNameEntityMap.find(nameID);
		if (it != mNameEntityMap.end())
		{
			Debug::Warning("Duplicate entity name:" + name);
			return it->second;
		}

		Entity entity = CreateEntity();
		mNames.Create(entity)->SetString(name);
		mNameEntityMap[nameID] = entity;

		return entity;
	}

	Entity Scene::GetEntityByName(const StringID& name)
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
		mObjects.Remove(entity);
		mObjectAABBs.Remove(entity);
		mTransforms.Remove(entity);

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