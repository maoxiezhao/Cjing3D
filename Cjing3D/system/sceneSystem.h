#pragma once

#include "common\common.h"
#include "system\component\transform.h"
#include "system\component\camera.h"
#include "system\component\mesh.h"
#include "system\component\object.h"
#include "system\component\material.h"

#include <map>

namespace Cjing3D {

	class Scene
	{
	public:
		Scene();

		void Update(F32 deltaTime);
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
		void UpdateSceneTransformSystem();
		void UpdateSceneObjectSystem();

	public:
		std::map<StringID, ECS::Entity> mNameEntityMap;

		ECS::ComponentManager<StringID> mNames;
		ECS::ComponentManager<MeshComponent> mMeshes;
		ECS::ComponentManager<MaterialComponent> mMaterials;
		ECS::ComponentManager<ObjectComponent> mObjects;
		ECS::ComponentManager<AABB> mObjectAABBs;
		ECS::ComponentManager<TransformComponent> mTransforms;

		AABB mSceneAABB;

		static Scene& GetScene()
		{
			static Scene scene;
			return scene;
		}
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
	