#pragma once

#include "common\common.h"
#include "system\component\genericType.h"
#include "system\component\transform.h"
#include "system\component\camera.h"
#include "system\component\mesh.h"
#include "system\component\object.h"
#include "system\component\material.h"
#include "system\component\light.h"
#include "system\component\terrain.h"
#include "system\component\animation.h"
#include "system\component\weather.h"

#include "helper\archive.h"

#include <map>
#include <tuple>

namespace Cjing3D {

	// TODO
	using ComponentManagerTypesConst = std::tuple<
		const ECS::ComponentManager<NameComponent>*,
		const ECS::ComponentManager<TransformComponent>*,
		const ECS::ComponentManager<HierarchyComponent>*,
		const ECS::ComponentManager<MaterialComponent>*,
		const ECS::ComponentManager<MeshComponent>*,
		const ECS::ComponentManager<ObjectComponent>*,
		const ECS::ComponentManager<AABBComponent>*,
		const ECS::ComponentManager<LightComponent>*,
		const ECS::ComponentManager<AABBComponent>*,
		const ECS::ComponentManager<TerrainComponent>*,
		const ECS::ComponentManager<ArmatureComponent>*,
		const ECS::ComponentManager<AnimationComponent>*,
		const ECS::ComponentManager<WeatherComponent>*
	>;

	using ComponentManagerTypes = std::tuple<
		ECS::ComponentManager<NameComponent>*,
		ECS::ComponentManager<TransformComponent>*,
		ECS::ComponentManager<HierarchyComponent>*,
		ECS::ComponentManager<MaterialComponent>*,
		ECS::ComponentManager<MeshComponent>*,
		ECS::ComponentManager<ObjectComponent>*,
		ECS::ComponentManager<AABBComponent>*,
		ECS::ComponentManager<LightComponent>*,
		ECS::ComponentManager<AABBComponent>*,
		ECS::ComponentManager<TerrainComponent>*,
		ECS::ComponentManager<ArmatureComponent>*,
		ECS::ComponentManager<AnimationComponent>*,
		ECS::ComponentManager<WeatherComponent>*
	>;

	class Scene
	{
	public:
		Scene();

		void Update(F32 deltaTime);
		void Merge(Scene& scene);
		void Clear();

		// create function
		ECS::Entity CreateEntityTransform(const std::string& name);
		ECS::Entity CreateEntityMaterial(const std::string& name);
		ECS::Entity CreateEntityMesh(const std::string& name);
		ECS::Entity CreateEntityObject(const std::string& name);
		ECS::Entity CreateEntityLight(const std::string& name, 
			const F32x3& pos = { 0.0f, 0.0f, 0.0f }, 
			const F32x3& color = {1.0f, 1.0f, 1.0f},
			F32 energy = 1.0f,
			F32 range = 10.0f
		);
		ECS::Entity CreateEntityTerrain(const std::string& name, U32 width, U32 height, U32 elevation);
		ECS::Entity CreateArmature(const std::string& name);
		ECS::Entity CreateAnimation(const std::string& name);
		ECS::Entity CreateWeather(const std::string& name);

		// create by entity
		NameComponent& GetOrCreateNameByEntity(ECS::Entity entity);
		TransformComponent& GetOrCreateTransformByEntity(ECS::Entity entity);
		LightComponent& GetOrCreateLightByEntity(ECS::Entity entity);

		ECS::Entity CreateEntityByName(const std::string& name);
		ECS::Entity GetEntityByName(const StringID& name);

		void RemoveEntity(ECS::Entity entity);
		// entity间的层级结构基于 hierarchy component
		void AttachEntity(ECS::Entity entity, ECS::Entity parent, bool alreadyInLocalSpace = false, bool detachIfAttached = true);
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
		ECS::ComponentManager<NameComponent> mNames;
		ECS::ComponentManager<TransformComponent> mTransforms;
		ECS::ComponentManager<HierarchyComponent> mHierarchies;
		ECS::ComponentManager<MaterialComponent> mMaterials;
		ECS::ComponentManager<MeshComponent> mMeshes;
		ECS::ComponentManager<ObjectComponent> mObjects;
		ECS::ComponentManager<AABBComponent> mObjectAABBs;
		ECS::ComponentManager<LightComponent> mLights;
		ECS::ComponentManager<AABBComponent> mLightAABBs;
		ECS::ComponentManager<TerrainComponent> mTerrains;
		ECS::ComponentManager<ArmatureComponent> mArmatures;
		ECS::ComponentManager<AnimationComponent> mAnimations;
		ECS::ComponentManager<WeatherComponent> mWeathers;

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
	void UpdateSceneTerrainSystem(Scene& scene);
	void UpdateSceneArmatureSystem(Scene& scene);
	void UpdateSceneAnimationSystem(Scene& scene);
}
	