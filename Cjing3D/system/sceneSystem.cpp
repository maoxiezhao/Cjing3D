#include "sceneSystem.h"
#include "resource\resourceManager.h"
#include "helper\profiler.h"

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
		ECS::ComponentManager<LayerComponent>::Initialize();
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
		ECS::ComponentManager<ParticleComponent>::Initialize();
	}

	void Scene::Uninitialize()
	{
		ECS::ComponentManager<NameComponent>::Uninitilize();
		ECS::ComponentManager<LayerComponent>::Uninitilize();
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
		ECS::ComponentManager<ParticleComponent>::Uninitilize();
	}

	void Scene::Update(F32 deltaTime)
	{
		SceneSystem::UpdateSceneAnimationSystem(*this);
		SceneSystem::UpdateSceneTransformSystem(*this);
		SceneSystem::UpdateHierarchySystem(*this);     // must update after transform
		SceneSystem::UpdateSceneArmatureSystem(*this);
		SceneSystem::UpdateSceneLightSystem(*this);
		SceneSystem::UpdateSceneObjectSystem(*this);
		SceneSystem::UpdateSceneTerrainSystem(*this);
		SceneSystem::UpdateSceneSoundSystem(*this);
		SceneSystem::UpdateSceneParticleSystem(*this, deltaTime);
	}

	void Scene::Merge(Scene & scene)
	{
		mNames.Merge(scene.mNames);
		mLayers.Merge(scene.mLayers);
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
		mLayers.Create(entity);

		return entity;
	}

	ECS::Entity Scene::CreateEntityLight(const std::string & name, const F32x3 & pos, const F32x3 & color, F32 energy, F32 range, LightComponent::LightType lightType)
	{
		auto entity = CreateEntityByName(name);
		TransformComponent& transform = *mTransforms.Create(entity);
		transform.Translate(XMConvert(pos));
		transform.Update();
		mLayers.Create(entity);
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

		mLayers.Create(entity);

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

	ECS::Entity Scene::CreateSound(const std::string& name, const std::string& filePath, bool is3DSound, const F32x3& pos)
	{
		auto entity = CreateEntityByName(name);

		if (is3DSound)
		{
			TransformComponent& transform = *mTransforms.Create(entity);
			transform.Translate(XMConvert(pos));
			transform.Update();
		}

		SoundComponent& sound = *mSounds.Create(entity);
		sound.mFileName = filePath;
		sound.mSoundResource = GetGlobalContext().gResourceManager->GetOrCreate<SoundResource>(filePath);

		if (sound.mSoundResource->mSound.IsValid()) {
			GetGlobalContext().gAudioManager->CreateInstance(sound.mSoundResource->mSound, sound.mSoundInstance);
		}

		return entity;
	}

	ECS::Entity Scene::CreateParticle(const std::string& name, const F32x3& pos)
	{
		auto entity = CreateEntityByName(name);
		TransformComponent& transform = *mTransforms.Create(entity);
		transform.Translate(XMConvert(pos));
		transform.Update();
	
		mParticles.Create(entity);
		mLayers.Create(entity);
		mMaterials.Create(entity)->SetBlendMode(BlendType_Alpha);

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

	ObjectComponent& Scene::GetOrCreateObjectByEntity(ECS::Entity entity)
	{
		auto object = mObjects.GetComponent(entity);
		if (object != nullptr) {
			return *object;
		}

		object = mObjects.Create(entity);
		mObjectAABBs.Create(entity);
		GetOrCreateTransformByEntity(entity);

		return *object;
	}

	LayerComponent& Scene::GetOrCreateLayerByEntity(ECS::Entity entity)
	{
		auto layerPtr = mLayers.GetComponent(entity);
		if (layerPtr != nullptr) {
			return *layerPtr;
		}

		return *mLayers.Create(entity);
	}

	ParticleComponent& Scene::GetOrCreateParticleByEntity(ECS::Entity entity)
	{
		auto particle = mParticles.GetComponent(entity);
		if (particle != nullptr) {
			return *particle;
		}

		particle = mParticles.Create(entity);
		mLayers.Create(entity);
		mMaterials.Create(entity)->SetBlendMode(BlendType_Alpha);
		GetOrCreateTransformByEntity(entity);

		return *particle;
	}

	SoundComponent& Scene::GetOrCreateSoundByEntity(ECS::Entity entity)
	{
		auto sound = mSounds.GetComponent(entity);
		if (sound != nullptr) {
			return *sound;
		}

		sound = mSounds.Create(entity);
		GetOrCreateTransformByEntity(entity);

		return *sound;
	}

	TerrainComponent& Scene::GetOrCreateTerrainByEntity(ECS::Entity entity)
	{
		auto terrain = mTerrains.GetComponent(entity);
		if (terrain == nullptr) {
			return *terrain;
		}

		terrain = mTerrains.Create(entity);
		mLayers.Create(entity);
		GetOrCreateTransformByEntity(entity);

		return *terrain;
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
		if (nameID != StringID::EMPTY)
		{
			mNames.Create(entity)->SetName(name);
			mNameEntityMap[nameID] = entity;
		}

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

	bool Scene::RenameEntity(ECS::Entity entity, const std::string& name)
	{
		// 暂时不使用重名检测
		if (false)
		{
			auto it = mNameEntityMap.find(name);
			if (it != mNameEntityMap.end())
			{
				Debug::Warning("Duplicate entity name:" + name);
				return false;
			}
		}

		auto nameComponentPtr = mNames.GetComponent(entity);
		if (nameComponentPtr == nullptr) {
			nameComponentPtr = mNames.Create(entity);
		}

		nameComponentPtr->SetName(name);
		return true;
	}

	void Scene::RemoveEntity(Entity entity)
	{
		DetachEntity(entity);

		if (mNames.Contains(entity))
		{
			auto nameComponent = mNames.GetComponent(entity);
			if (nameComponent != nullptr)
			{
				mNameEntityMap.erase(nameComponent->GetName());
			}
			mNames.Remove(entity);
		}

		// use std::apply and fold expression, and it needs c++17
		auto allComponentManagers = GetAllComponentManagers();
		std::apply([entity](auto&&... componentManager) {
			return (componentManager->Remove(entity), ...); },
			allComponentManagers
		);
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

		auto hierarchy = mHierarchies.Create(entity);
		hierarchy->mParent = parent;

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

		// handle layer
		auto parentLayer = mLayers.GetOrCreateComponent(parent);
		auto childLayer = mLayers.GetOrCreateComponent(entity);
		hierarchy->mChildBindLayerMask = childLayer->GetLayerMask();
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

			auto layer = mLayers.GetComponent(entity);
			if (layer != nullptr) {
				layer->SetLayerMask(hierarchy->mChildBindLayerMask);
			}

			mHierarchies.RemoveAndKeepSorted(entity);
		}
	}

	void Scene::DetachEntityChildren(ECS::Entity entity)
	{
		if (entity == ECS::INVALID_ENTITY) {
			return;
		}

		std::vector<ECS::Entity> removed;
		for (const auto& hierarchy : mHierarchies.GetComponents())
		{
			if (hierarchy->mParent == entity)
			{
				removed.push_back(hierarchy->GetCurrentEntity());
			}
		}

		for (const auto& entity : removed) 
		{
			DetachEntity(entity);
		}
	}

	bool Scene::IsEntityAttached(ECS::Entity entity) const
	{
		auto hierarchy = mHierarchies.GetComponent(entity);
		return (hierarchy != nullptr && hierarchy->mParent != INVALID_ENTITY);
	}

	ECS::Entity Scene::DuplicateEntity(ECS::Entity entity)
	{
		ECS::Entity newEntity = CreateEntity();
		auto allComponentManagers = GetAllComponentManagers();
		std::apply([entity, newEntity](auto&&... componentManager) {
			return (componentManager->DuplicateComponent(entity, newEntity), ...); },
			allComponentManagers
		);
		return newEntity;
	}

	void Scene::SetEntityLayerMask(ECS::Entity entity, U32 layerMask)
	{
		auto layer = mLayers.GetOrCreateComponent(entity);
		layer->SetLayerMask(layerMask);
	}

	std::string Scene::GetEntityName(ECS::Entity entity) const
	{
		auto name = mNames.GetComponent(entity);
		return name != nullptr ? name->GetString() : (entity != INVALID_ENTITY ? std::to_string(entity) : "");
	}

	void Scene::RemoveComponentByType(ECS::Entity entity, ComponentType type)
	{
		switch (type)
		{
		case Cjing3D::ComponentType_Camera:
			break;
		case Cjing3D::ComponentType_Ojbect:
			mObjects.Remove(entity);
			mObjectAABBs.Remove(entity);
			break;
		case Cjing3D::ComponentType_Mesh:
			mMeshes.Remove(entity);
			break;
		case Cjing3D::ComponentType_Material:
			mMeshes.Remove(entity);
			break;
		case Cjing3D::ComponentType_Light:
			mLights.Remove(entity);
			mLightAABBs.Remove(entity);
			break;
		case Cjing3D::ComponentType_Terrain:
			mTerrains.Remove(entity);
			break;
		case Cjing3D::ComponentType_Animation:
			mAnimations.Remove(entity);
			break;
		case Cjing3D::ComponentType_Armature:
			mArmatures.Remove(entity);
			break;
		case Cjing3D::ComponentType_Weather:
			mWeathers.Remove(entity);
			break;
		case Cjing3D::ComponentType_Sound:
			mSounds.Remove(entity);
			break;
		case Cjing3D::ComponentType_Particle:
			mParticles.Remove(entity);
			break;
		default:
			break;
		}
	}

	Scene::PickResult Scene::MousePickObjects(const U32x2& mousePos, const U32 layerMask, bool triangleIntersect)
	{
		return PickObjects(Renderer::GetMainCameraMouseRay(mousePos), layerMask, triangleIntersect);
	}

	Scene::PickResult Scene::PickObjects(const Ray& ray, const U32 layerMask, bool triangleIntersect)
	{
		auto& objects = mObjects.GetEntities();
		return PickObjects(objects, ray, layerMask, triangleIntersect);
	}

	Scene::PickResult Scene::PickObjects(const std::vector<ECS::Entity>& objects, const Ray& ray, const U32 layerMask, bool triangleIntersect)
	{
		PickResult ret;
		if (mObjects.Empty()) {
			return ret;
		}

		XMVECTOR originV = XMLoadFloat3(&ray.mOrigin);
		XMVECTOR dirV = XMLoadFloat3(&ray.mDirection);

		PROFILER_BEGIN_CPU_BLOCK("CPU_RAY_PICKING");
		for (const auto& entity : objects)
		{
			const ObjectComponent* object = mObjects.GetComponent(entity);
			if (object == nullptr || object->mMeshID == INVALID_ENTITY) {
				continue;
			}

			const LayerComponent* layer = mLayers.GetComponent(entity);
			if (layer != nullptr && (layer->GetLayerMask() & layerMask) == 0) {
				continue;
			}

			const AABBComponent* aabbComponent = mObjectAABBs.GetComponent(entity);
			if (aabbComponent == nullptr) {
				continue;
			}

			F32 currentT = 0.0f;
			const AABB& aabb = aabbComponent->GetAABB();
			if (!ray.Intersects(aabb, &currentT)) {
				continue;
			}

			if (!triangleIntersect)
			{
				// 如果不遍历mesh，则直接处理和AABB的相交点

				XMVECTOR hitPos = originV + dirV * currentT;
				F32 distance = XMDistance(originV, hitPos);
				if (distance < ret.distance)
				{
					ret.distance = distance;
					ret.entity = entity;
					ret.position = XMStore<F32x3>(hitPos);
				}

				continue;
			}

			MeshComponent* mesh = mMeshes.GetComponent(object->mMeshID);
			if (mesh == nullptr) {
				continue;
			}

			// 对模型进行mesh逐三角形射线检测，将射线转换到模型空间
			// 依次遍历所有三角形，判断是否和三角形相交
			XMMATRIX objMat = XMMatrixIdentity();
			TransformComponent* transform = mTransforms.GetComponent(entity);
			if (transform != nullptr) {
				objMat = XMLoadFloat4x4(&transform->GetWorldTransform());
			}

			XMMATRIX objInvMat = XMMatrixInverse(nullptr, objMat);
			const XMVECTOR rayOriginLocal = XMVector3Transform(originV, objInvMat);
			const XMVECTOR rayDirLocal = XMVector3Normalize(XMVector3TransformNormal(dirV, objInvMat));

			for (const auto& subset : mesh->mSubsets)
			{
				for (size_t i = 0; i < subset.mIndexCount; i+=3)
				{
					const U32 offset = subset.mIndexOffset;
					const U32 i0 = mesh->mIndices[offset + i + 0];
					const U32 i1 = mesh->mIndices[offset + i + 1];
					const U32 i2 = mesh->mIndices[offset + i + 2];
				
					// 这里需要考虑Skinning，目前暂时不不支持
					XMVECTOR p0 = XMLoad(mesh->mVertexPositions[i0]);
					XMVECTOR p1 = XMLoad(mesh->mVertexPositions[i1]);
					XMVECTOR p2 = XMLoad(mesh->mVertexPositions[i2]);
				
					F32 distance = FLT_MAX;
					F32x2 bary = F32x2(0.0f, 0.0f);
					if (TriangleRayInstersects(rayOriginLocal, rayDirLocal, p0, p1, p2, distance, bary))
					{
						// transform hitpos to world space
						XMVECTOR hitPos = XMVector3Transform(rayOriginLocal + rayDirLocal * distance, objMat);
						distance = XMDistance(hitPos, originV);
						
						if (distance < ret.distance)
						{
							ret.distance = distance;
							ret.entity = entity;
							ret.position = XMStore<F32x3>(hitPos);
							ret.bary = bary;
						}
					}
				}
			}
		}
		PROFILER_END_BLOCK();

		return ret;
	}

	Scene::PickResult Scene::PickObjectsByBullet(const Ray& ray, const U32 layerMask)
	{
		return PickResult();
	}

	ComponentManagerTypesConst Scene::GetAllComponentManagers()const
	{
		ComponentManagerTypesConst t = std::make_tuple(
			&mNames,
			&mLayers,
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
			&mSounds,
			&mParticles
		);
		return t;
	}

	ComponentManagerTypes Scene::GetAllComponentManagers()
	{
		ComponentManagerTypes t = std::make_tuple(
			&mNames,
			&mLayers,
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
			&mSounds,
			&mParticles
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

	ECS::Entity Scene::LoadModel(const std::string& path)
	{
		std::string extension = FileData::GetExtensionFromFilePath(path);
		if (extension == ".c3dscene") {
			return Scene::GetScene().LoadSceneFromArchive(path);
		}
		else if (extension == ".obj") {
			return ModelImporter::ImportModelObj(path, *this);
		}
		else if (extension == ".gltf") {
			return ModelImporter::ImportModelGLTF(path, *this);
		}
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
			mLayers.Create(root);

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

	void SceneSystem::UpdateHierarchySystem(Scene& scene)
	{
		// UpdateFromParent必须保证parent比child先执行
		// 所以mHierarchies中的排序必须保证parent在child前面

		for (size_t index = 0; index < scene.mHierarchies.GetCount(); index++)
		{
			auto hierarchy = scene.mHierarchies[index];
			auto entity = hierarchy->GetCurrentEntity();

			auto parentTransform = scene.mTransforms.GetComponent(hierarchy->mParent);
			auto childTransform = scene.mTransforms.GetComponent(entity);		
			if (parentTransform != nullptr && childTransform != nullptr) {
				childTransform->UpdateFromParent(*parentTransform);
			}

			auto parentLayer = scene.mLayers.GetComponent(hierarchy->mParent);
			auto childLayer = scene.mLayers.GetComponent(entity);
			if (parentLayer != nullptr && childLayer != nullptr) {
				childLayer->SetLayerMask(parentLayer->GetLayerMask() & hierarchy->mChildBindLayerMask);
			} 
		}
	}
}