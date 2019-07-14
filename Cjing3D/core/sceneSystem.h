#pragma once

#include "common\common.h"
#include "world\component\transform.h"
#include "world\component\camera.h"
#include "world\component\mesh.h"
#include "world\component\object.h"
#include "world\component\material.h"

#include <map>

namespace Cjing3D {

	class Scene
	{
	public:
		Scene();

		void Update();
		void Merge(Scene& scene);
		void Clear();

		// create function
		ECS::Entity CreateEntityMaterial(const std::string& name);
		ECS::Entity CreateEntityMesh(const std::string& name);
		ECS::Entity CreateEntityObject(const std::string& name);

		ECS::Entity CreateEntityByName(const std::string& name);
		ECS::Entity GetEntityByName(StringID name);

		void RemoveEntity(ECS::Entity entity);

		template<typename ComponentT>
		std::shared_ptr<ComponentT> GetComponent(ECS::Entity entity)
		{
			auto& manager = GetComponentManager<ComponentT>();
			return manager.GetComponent(entity);
		}

		template<typename ComponentT>
		std::shared_ptr<ComponentT> GetComponentByIndex(U32 index)
		{
			auto& manager = GetComponentManager<ComponentT>();
			if (index >= manager.GetCount())
				return nullptr;

			return manager[index];
		}

		template<typename ComponentT>
		ECS::Entity GetEntityByIndex(U32 index)
		{
			auto& manager = GetComponentManager<ComponentT>();
			if (index >= manager.GetCount())
				return ECS::INVALID_ENTITY;

			return manager.GetEntityByIndex(index);
		}

		template<typename ComponentT>
		ECS::ComponentManager<ComponentT>& GetComponentManager();

	private:
		void UpdateSceneTransform();
		void UpdateSceneObject();

	public:
		std::map<StringID, ECS::Entity> mNameEntityMap;

		ECS::ComponentManager<StringID> mNames;
		ECS::ComponentManager<MeshComponent> mMeshes;
		ECS::ComponentManager<MaterialComponent> mMaterials;
		ECS::ComponentManager<ObjectComponent> mObjects;
		ECS::ComponentManager<AABB> mObjectAABBs;
		ECS::ComponentManager<TransformComponent> mTransforms;

		AABB mSceneAABB;
	};

	template<>
	inline ECS::ComponentManager<MeshComponent>& Scene::GetComponentManager()
	{
		return mMeshes;
	}

	template<>
	inline ECS::ComponentManager<MaterialComponent>& Scene::GetComponentManager()
	{
		return mMaterials;
	}

	template<>
	inline ECS::ComponentManager<ObjectComponent>& Scene::GetComponentManager()
	{
		return mObjects;
	}

	template<>
	inline ECS::ComponentManager<AABB>& Scene::GetComponentManager()
	{
		return mObjectAABBs;
	}

	template<>
	inline ECS::ComponentManager<TransformComponent>& Scene::GetComponentManager()
	{
		return mTransforms;
	}
}
	