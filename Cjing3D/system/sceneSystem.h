#pragma once

#include "common\common.h"
#include "system\component\transform.h"
#include "system\component\camera.h"
#include "system\component\mesh.h"
#include "system\component\object.h"
#include "system\component\material.h"
#include "system\component\light.h"

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
		ECS::Entity CreateEntityLight(const std::string& name, 
			const F32x3& pos = { 0.0f, 0.0f, 0.0f }, 
			const F32x3& color = {1.0f, 1.0f, 1.0f},
			F32 energy = 1.0f,
			F32 range = 10.0f
		);

		// create by entity
		TransformComponent& GetOrCreateTransformByEntity(ECS::Entity entity);
		LightComponent& GetOrCreateLightByEntity(ECS::Entity entity);

		ECS::Entity CreateEntityByName(const std::string& name);
		ECS::Entity GetEntityByName(const StringID& name);

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

	public:
		std::map<StringID, ECS::Entity> mNameEntityMap;

		ECS::ComponentManager<StringID> mNames;
		ECS::ComponentManager<MeshComponent> mMeshes;
		ECS::ComponentManager<MaterialComponent> mMaterials;
		ECS::ComponentManager<ObjectComponent> mObjects;
		ECS::ComponentManager<AABB> mObjectAABBs;
		ECS::ComponentManager<TransformComponent> mTransforms;
		ECS::ComponentManager<LightComponent> mLights;
		ECS::ComponentManager<AABB> mLightAABBs;

		AABB mSceneAABB;

		static Scene& GetScene()
		{
			static Scene scene;
			return scene;
		}
	};

	#include "system\sceneSystem.inl"

	//  Scene System Update
	void UpdateSceneTransformSystem(Scene& scene);
	void UpdateSceneObjectSystem(Scene& scene);
	void UpdateSceneLightSystem(Scene& scene);
}
	