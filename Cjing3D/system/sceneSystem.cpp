#include "sceneSystem.h"
#include "resource\resourceManager.h"

namespace Cjing3D
{
	using namespace ECS;

	Scene::Scene()
	{
	}

	Scene::~Scene()
	{
	}

	void Scene::Initialize()
	{
		ECS::ComponentManager<NameComponent>::Initialize();
		ECS::ComponentManager<TransformComponent>::Initialize();
		ECS::ComponentManager<HierarchyComponent>::Initialize();
		ECS::ComponentManager<MaterialComponent>::Initialize();
		ECS::ComponentManager<MeshComponent>::Initialize();
		ECS::ComponentManager<ObjectComponent>::Initialize();
		ECS::ComponentManager<LightComponent>::Initialize();
		ECS::ComponentManager<AABBComponent>::Initialize();
		ECS::ComponentManager<TerrainComponent>::Initialize();
		ECS::ComponentManager<ArmatureComponent>::Initialize();
		ECS::ComponentManager<AnimationComponent>::Initialize();
		ECS::ComponentManager<WeatherComponent>::Initialize();
	}

	void Scene::Uninitialize()
	{
		ECS::ComponentManager<NameComponent>::Uninitilize();
		ECS::ComponentManager<TransformComponent>::Uninitilize();
		ECS::ComponentManager<HierarchyComponent>::Uninitilize();
		ECS::ComponentManager<MaterialComponent>::Uninitilize();
		ECS::ComponentManager<MeshComponent>::Uninitilize();
		ECS::ComponentManager<ObjectComponent>::Uninitilize();
		ECS::ComponentManager<LightComponent>::Uninitilize();
		ECS::ComponentManager<AABBComponent>::Uninitilize();
		ECS::ComponentManager<TerrainComponent>::Uninitilize();
		ECS::ComponentManager<ArmatureComponent>::Uninitilize();
		ECS::ComponentManager<AnimationComponent>::Uninitilize();
		ECS::ComponentManager<WeatherComponent>::Uninitilize();
	}

	void Scene::Update(F32 deltaTime)
	{
		UpdateSceneAnimationSystem(*this);
		UpdateSceneTransformSystem(*this);
		UpdateHierarchySystem(*this);     // must update after transform
		UpdateSceneArmatureSystem(*this);
		UpdateSceneLightSystem(*this);
		UpdateSceneObjectSystem(*this);
		UpdateSceneTerrainSystem(*this);
		UpdateSceneSoundSystem(*this);
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
		mHierarchies.Merge(scene.mHierarchies);
		mTerrains.Merge(scene.mTerrains);
		mArmatures.Merge(scene.mArmatures);
		mAnimations.Merge(scene.mAnimations);
	}

	void Scene::Clear()
	{
		// use std::apply and fold expression, and it needs c++17
		auto allComponentManagers = GetAllComponentManagers();
		std::apply([](auto&&... componentManager) {
			return (componentManager->Clear(), ...); },
			allComponentManagers
		);
	}

	ECS::Entity Scene::CreateEntityTransform(const std::string& name)
	{
		auto entity = CreateEntityByName(name);
		mTransforms.Create(entity);

		return entity;
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

	ECS::Entity Scene::CreateEntityLight(const std::string & name, const F32x3 & pos, const F32x3 & color, F32 energy, F32 range, LightComponent::LightType lightType)
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
		light.SetLightType(lightType);

		return entity;
	}

	ECS::Entity Scene::CreateEntityTerrain(const std::string& name, U32 width, U32 height, U32 elevation)
	{
		auto entity = CreateEntityByName(name);
		TransformComponent& transform = *mTransforms.Create(entity);
		transform.Update();

		TerrainComponent& terrain = *mTerrains.Create(entity);
		terrain.SetElevation(elevation);
		terrain.SetTerrainSize(width, height);

		return entity;
	}

	ECS::Entity Scene::CreateArmature(const std::string& name)
	{
		auto entity = CreateEntityByName(name);
		TransformComponent& transform = *mTransforms.Create(entity);
		transform.Update();

		ArmatureComponent& armature = *mArmatures.Create(entity);

		return entity;
	}

	ECS::Entity Scene::CreateAnimation(const std::string& name)
	{
		auto entity = CreateEntityByName(name);
		mAnimations.Create(entity);

		return entity;
	}

	ECS::Entity Scene::CreateWeather(const std::string& name)
	{
		auto entity = CreateEntityByName(name);
		mWeathers.Create(entity);

		return entity;
	}

	ECS::Entity Scene::CreateSound(const std::string& name, const std::string& filePath, const F32x3& pos)
	{
		auto entity = CreateEntityByName(name);
		TransformComponent& transform = *mTransforms.Create(entity);
		transform.Translate(XMConvert(pos));
		transform.Update();

		SoundComponent& sound = *mSounds.Create(entity);
		sound.mFileName = filePath;
		sound.mSoundResource = GlobalGetSubSystem<ResourceManager>().GetOrCreate<SoundResource>(filePath);

		if (sound.mSoundResource->mSound.IsValid()) {
			GlobalGetSubSystem<Audio::AudioManager>().CreateInstance(sound.mSoundResource->mSound, sound.mSoundInstance);
		}

		return entity;
	}

	NameComponent& Scene::GetOrCreateNameByEntity(ECS::Entity entity)
	{
		auto nameComponentPtr = mNames.GetComponent(entity);
		if (nameComponentPtr != nullptr) {
			return *nameComponentPtr;
		}

		NameComponent& nameComponent = *mNames.Create(entity);
		return nameComponent;
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

	// 仅创建light相关component
	LightComponent & Scene::GetOrCreateLightByEntity(ECS::Entity entity)
	{
		auto lightPtr = mLights.GetComponent(entity);
		if (lightPtr != nullptr) {
			return *lightPtr;
		}

		mLightAABBs.Create(entity);
		LightComponent& light = *mLights.Create(entity);
		light.SetLightType(LightComponent::LightType_Point);

		GetOrCreateTransformByEntity(entity);

		return light;
	}

	ECS::Entity Scene::CreateEntityByName(const std::string & name)
	{
		auto nameID = StringID(name);

		// 暂时移除名字寻址功能，所以名字可以重复存在
		if (false)
		{
			auto it = mNameEntityMap.find(nameID);
			if (it != mNameEntityMap.end())
			{
				Debug::Warning("Duplicate entity name:" + name);
				return it->second;
			}
		}

		Entity entity = CreateEntity();
		mNames.Create(entity)->SetName(name);
		mNameEntityMap[nameID] = entity;

		return entity;
	}

	Entity Scene::GetEntityByName(const StringID& name)
	{
		if (false)
		{
			auto it = mNameEntityMap.find(name);
			if (it != mNameEntityMap.end())
			{
				return it->second;
			}
		}

		return ECS::INVALID_ENTITY;
	}

	void Scene::RemoveEntity(Entity entity)
	{
		DetachEntity(entity);

		mMaterials.Remove(entity);
		mMeshes.Remove(entity);
		mObjects.Remove(entity);
		mObjectAABBs.Remove(entity);
		mTransforms.Remove(entity);
		mLightAABBs.Remove(entity);
		mLights.Remove(entity);
		mTerrains.Remove(entity);
		mArmatures.Remove(entity);
		mAnimations.Remove(entity);
		mWeathers.Remove(entity);

		if (mNames.Contains(entity))
		{
			auto nameComponent = mNames.GetComponent(entity);
			if (nameComponent != nullptr)
			{
				mNameEntityMap.erase(nameComponent->GetName());
			}
			mNames.Remove(entity);
		}
	}

	void Scene::AttachEntity(ECS::Entity entity, ECS::Entity parent, bool alreadyInLocalSpace, bool detachIfAttached)
	{
		Debug::CheckAssertion(entity != parent, "Attach entity to self.");

		if (mHierarchies.Contains(entity)) 
		{
			if (!detachIfAttached) {
				return;
			}
			DetachEntity(entity);
		}

		auto hierarchy = mHierarchies.Create(entity)->mParent = parent;

		// fix tree which keep parent before child
		if (mHierarchies.GetCount() > 1)
		{
			for (int i = mHierarchies.GetCount() - 1; i > 0; i--)
			{
				auto parentHierachy = mHierarchies.GetEntityByIndex(i);
				for (int j = 0; j < i; j++)
				{
					auto childHierachy = mHierarchies.GetComponentByIndex(j);
					if (childHierachy->mParent == parentHierachy)
					{
						mHierarchies.MoveInto(i, j);
						i++;	// check same index again
						break;
					}
				}
			}
		}

		// handle transform
		auto parentTransform = mTransforms.GetOrCreateComponent(parent);
		auto childTransform = mTransforms.GetOrCreateComponent(entity);
		if (!alreadyInLocalSpace)
		{
			// 如果不在local space，则需要乘上父级的逆矩阵转换到local space
			XMMATRIX inverseM = XMMatrixInverse(nullptr, XMLoadFloat4x4(&parentTransform->GetWorldTransform()));
			childTransform->UpdateByTransform(inverseM);
			childTransform->Update();
		}

		childTransform->UpdateFromParent(*parentTransform);
	}

	void Scene::DetachEntity(ECS::Entity entity)
	{
		auto hierarchy = mHierarchies.GetComponent(entity);
		if (hierarchy != nullptr)
		{
			auto transform = mTransforms.GetComponent(entity);
			if (transform != nullptr) {
				transform->ApplyTransform();
			}

			mHierarchies.RemoveAndKeepSorted(entity);
		}
	}

	ComponentManagerTypesConst Scene::GetAllComponentManagers()const
	{
		ComponentManagerTypesConst t = std::make_tuple(
			&mNames,
			&mTransforms,
			&mHierarchies,
			&mMaterials,
			&mMeshes,
			&mObjects,
			&mObjectAABBs,
			&mLights,
			&mLightAABBs,
			&mTerrains,
			&mArmatures,
			&mAnimations,
			&mWeathers,
			&mSounds
		);
		return t;
	}

	ComponentManagerTypes Scene::GetAllComponentManagers()
	{
		ComponentManagerTypes t = std::make_tuple(
			&mNames,
			&mTransforms,
			&mHierarchies,
			&mMaterials,
			&mMeshes,
			&mObjects,
			&mObjectAABBs,
			&mLights,
			&mLightAABBs,
			&mTerrains,
			&mArmatures,
			&mAnimations,
			&mWeathers,
			&mSounds
		);
		return t;
	}

	U32 Scene::GetEntityCount()const
	{
		U32 entityCount = 0;

		// use std::apply and fold expression, and it needs c++17
		auto allComponentManagers = GetAllComponentManagers();
		entityCount = std::apply([](auto&&... componentManager) {
			return (componentManager->GetCount() + ...); },
			allComponentManagers
		);

		return entityCount;
	}

	ECS::Entity Scene::LoadSceneFromArchive(const std::string& path)
	{
		Logger::Info("LoadSceneFromArchive, path" + path);

		Scene newScene;
		ECS::Entity root = ECS::INVALID_ENTITY;

		// 从目标文件中序列化场景
		// 场景为一系列entity集合，以transform树结构为层级结构
		Archive archive(path, ArchiveMode::ArchiveMode_Read);
		if (archive.IsOpen())
		{
			archive >> newScene;

			root = CreateEntity();
			mTransforms.Create(root);

			// 每一个没有父级结构transform指向root
			for (auto& entity : newScene.mTransforms.GetEntities())
			{
				if (!newScene.mHierarchies.Contains(entity)) {
					newScene.AttachEntity(entity, root, true);
				}
			}

			newScene.Update(0);
		}

		if (root != ECS::INVALID_ENTITY) {
			GetScene().Merge(newScene);
		}
		return root;
	}

	void Scene::SaveSceneToArchive(const std::string& path)
	{
		Archive archive(path, ArchiveMode::ArchiveMode_Write);
		if (archive.IsOpen()) {
			archive << (*this);
		}
	}

	void UpdateHierarchySystem(Scene& scene)
	{
		// UpdateFromParent必须保证parent比child先执行
		// 所以mHierarchies中的排序必须保证parent在child前面

		for (size_t index = 0; index < scene.mHierarchies.GetCount(); index++)
		{
			auto hierarchy = scene.mHierarchies[index];
			auto entity = hierarchy->GetCurrentEntity();

			auto parentTransform = scene.mTransforms.GetComponent(hierarchy->mParent);
			auto childTransform = scene.mTransforms.GetComponent(entity);
			
			if (parentTransform == nullptr || childTransform == nullptr) {
				continue;
			}

			childTransform->UpdateFromParent(*parentTransform);
		}
	}
}