#pragma once

#include "common\common.h"
#include "system\component\transform.h"
#include "system\component\camera.h"
#include "system\component\mesh.h"
#include "system\component\object.h"
#include "system\component\material.h"
#include "system\component\light.h"

#include "helper\archive.h"

#include <map>
#include <tuple>

namespace Cjing3D {

	//  Hierarchy用于组织Transform之间的层级关系
	class HierarchyComponent : public Component
	{
	public:
		HierarchyComponent() : Component(ComponentType_HierarchyComponent) {}

		// parent entity
		ECS::Entity mParent = ECS::INVALID_ENTITY;
		// parent inverse world matrix
		XMMATRIX mParentBindInverseWorld = XMMatrixIdentity();

		virtual void Serialize(Archive& archive, U32 seed = 0);
		virtual void Unserialize(Archive& archive)const;
	};

	// TODO
	using ComponentManagerTypesConst = std::tuple<
		const ECS::ComponentManager<StringID>*,
		const ECS::ComponentManager<TransformComponent>*,
		const ECS::ComponentManager<HierarchyComponent>*,
		const ECS::ComponentManager<MaterialComponent>*,
		const ECS::ComponentManager<MeshComponent>*,
		const ECS::ComponentManager<ObjectComponent>*,
		const ECS::ComponentManager<AABB>*,
		const ECS::ComponentManager<LightComponent>*,
		const ECS::ComponentManager<AABB>*
	>;

	using ComponentManagerTypes = std::tuple<
		ECS::ComponentManager<StringID>*,
		ECS::ComponentManager<TransformComponent>*,
		ECS::ComponentManager<HierarchyComponent>*,
		ECS::ComponentManager<MaterialComponent>*,
		ECS::ComponentManager<MeshComponent>*,
		ECS::ComponentManager<ObjectComponent>*,
		ECS::ComponentManager<AABB>*,
		ECS::ComponentManager<LightComponent>*,
		ECS::ComponentManager<AABB>*
	>;

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

		// entity间的层级结构基于 hierarchy component
		void AttachEntity(ECS::Entity entity, ECS::Entity parent);
		void DetachEntity(ECS::Entity entity);

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
		template<typename ComponentT>
		const ECS::ComponentManager<ComponentT>& GetComponentManager()const;
		ComponentManagerTypesConst GetAllComponentManagers()const;
		ComponentManagerTypes GetAllComponentManagers();

		U32 GetEntityCount()const;

		ECS::Entity LoadSceneFromArchive(const std::string& path);
		void SaveSceneToArchive(const std::string& path);

		void Serialize(Archive& archive);
		void Unserialize(Archive& archive)const;

	public:
		std::map<StringID, ECS::Entity> mNameEntityMap;

		// TODO: componentManager集合的维护非常繁琐，后续会重新设计结构或者用宏封装一层
		ECS::ComponentManager<StringID> mNames;
		ECS::ComponentManager<TransformComponent> mTransforms;
		ECS::ComponentManager<HierarchyComponent> mHierarchies;
		ECS::ComponentManager<MaterialComponent> mMaterials;
		ECS::ComponentManager<MeshComponent> mMeshes;
		ECS::ComponentManager<ObjectComponent> mObjects;
		ECS::ComponentManager<AABB> mObjectAABBs;
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
	void UpdateHierarchySystem(Scene& scene);
	void UpdateSceneTransformSystem(Scene& scene);
	void UpdateSceneObjectSystem(Scene& scene);
	void UpdateSceneLightSystem(Scene& scene);
}
	