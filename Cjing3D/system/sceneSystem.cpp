#include "sceneSystem.h"

namespace Cjing3D
{
	using namespace ECS;

	Scene::Scene()
	{
	}

	void Scene::Update(F32 deltaTime)
	{
		UpdateSceneTransformSystem();
		UpdateSceneObjectSystem();
	}

	void Scene::Merge(Scene & scene)
	{
		mNames.Merge(scene.mNames);
		mMeshes.Merge(scene.mMeshes);
		mMaterials.Merge(scene.mMaterials);
		mObjects.Merge(scene.mObjects);
		mObjectAABBs.Merge(scene.mObjectAABBs);
		mTransforms.Merge(scene.mTransforms);
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

	void Scene::UpdateSceneTransformSystem()
	{
		for (size_t index = 0; index < mTransforms.GetCount(); index++)
		{
			auto transform = mTransforms[index];
			transform->Update();
		}
	}

	void Scene::UpdateSceneObjectSystem()
	{
		AABB sceneAABB;

		for (size_t i = 0; i < mObjects.GetCount(); i++)
		{
			std::shared_ptr<ObjectComponent> object = mObjects[i];
			std::shared_ptr<AABB> aabb = mObjectAABBs[i];

			if (object == nullptr || aabb == nullptr) {
				continue;
			}

			// 遍历所有的object根据对应的transform，来更新AABB, 更新object的位置

			if (object->mMeshID != ECS::INVALID_ENTITY)
			{
				Entity entity = mObjects.GetEntityByIndex(i);
				std::shared_ptr<MeshComponent> mesh = mMeshes.GetComponent(object->mMeshID);

				const auto transformIndex = mTransforms.GetEntityIndex(entity);
				const auto transform = mTransforms[transformIndex];
				if (mesh != nullptr)
				{
					XMFLOAT4X4 worldMatrix = transform->GetWorldTransform();
					auto meshAABB = mesh->mAABB.GetByTransforming(worldMatrix);
					aabb->CopyFromOther(meshAABB);

					XMFLOAT4X4 boxMatrix;
					XMStoreFloat4x4(&boxMatrix, aabb->GetBoxMatrix());

					// update center pos
					XMFLOAT3 centerPos = *((XMFLOAT3*)&boxMatrix._41);
					object->mCenter = XMStore<F32x3>(XMLoadFloat3(&centerPos));

					for (auto& subset : mesh->mSubsets)
					{
						auto material = mMaterials.GetComponent(subset.mMaterialID);
						if (material != nullptr)
						{
							object->SetCastingShadow(material->IsCastingShadow());
						}
					}

					sceneAABB = AABB::Union(sceneAABB, *aabb);
				}
			}
		}

		mSceneAABB.CopyFromOther(sceneAABB);
	}

	
}