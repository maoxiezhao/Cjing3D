#pragma once

#include "sceneSystemInclude.h"

namespace Cjing3D {

	class Scene
	{
	public:
		Scene();
		~Scene();

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
			F32 range = 10.0f,
			LightComponent::LightType lightType = LightComponent::LightType_Point
		);
		ECS::Entity CreateEntityTerrain(const std::string& name, U32 width, U32 height, U32 elevation);
		ECS::Entity CreateArmature(const std::string& name);
		ECS::Entity CreateAnimation(const std::string& name);
		ECS::Entity CreateWeather(const std::string& name);
		ECS::Entity CreateSound(const std::string& name, const std::string& filePath, bool is3DSound = true, const F32x3& pos = { 0.0f, 0.0f, 0.0f });

		// create by entity
		NameComponent& GetOrCreateNameByEntity(ECS::Entity entity);
		TransformComponent& GetOrCreateTransformByEntity(ECS::Entity entity);
		LightComponent& GetOrCreateLightByEntity(ECS::Entity entity);

		ECS::Entity CreateEntityByName(const std::string& name);
		ECS::Entity GetEntityByName(const StringID& name);

		bool RenameEntity(ECS::Entity entity, const std::string& name);
		void RemoveEntity(ECS::Entity entity);
		// entity间的层级结构基于 hierarchy component
		void AttachEntity(ECS::Entity entity, ECS::Entity parent, bool alreadyInLocalSpace = false, bool detachIfAttached = true);
		void DetachEntity(ECS::Entity entity);

		struct PickResult
		{
			ECS::Entity entity = INVALID_ENTITY;
			F32 distance = FLT_MAX;
			F32x3 position = F32x3(0.0f, 0.0f, 0.0f);
			F32x2 bary = F32x2(0.0f, 0.0f);
		};
		PickResult MousePickObjects(const U32x2& mousePos);
		PickResult PickObjects(const Ray& ray);
		PickResult PickObjects(const Ray& ray, const std::vector<ECS::Entity>& objects, bool triangleIntersect = true);
		PickResult PickObjectsByBullet(const Ray& ray);

		template<typename ComponentT>
		ComponentT* GetComponent(ECS::Entity entity)
		{
			auto& manager = GetComponentManager<ComponentT>();
			return manager.GetComponent(entity);
		}

		template<typename ComponentT>
		ComponentT* GetComponentByIndex(U32 index)
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

		ECS::Entity LoadModel(const std::string& path);
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
		ECS::ComponentManager<SoundComponent> mSounds;

		AABB mSceneAABB;

		static Scene& GetScene()
		{
			static Scene scene;
			return scene;
		}

		static void Initialize();
		static void Uninitialize();
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
	void UpdateSceneSoundSystem(Scene& scene);
}
	