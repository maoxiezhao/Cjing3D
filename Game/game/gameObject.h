#pragma once

#include "definitions\definitions.h"
#include "system\sceneSystem.h"

namespace CjingGame
{
	enum GameObjectLayerMask
	{
		GameObjectLayerMask_Ground = 1 << 0,
		GameObjectLayerMask_Wall = 1 << 1,
	};

	class GameObject
	{
	public:
		GameObject();
		GameObject(const Cjing3D::StringID& name);
		virtual ~GameObject();

		void LoadFromScene(Cjing3D::Scene& scene);
		void RemoveFromScene();
		bool IsOnScene()const;

		void SetPosition(const F32x3& pos);
		void SetRotation(const F32x3& rotation);
		void SetScale(const F32x3& scale);
		void SetVisible(bool visible);

		void SetMeshFromModel(const std::string& path);
		void SetMesh(const ECS::Entity& meshEntity);

		GameObject Duplicate();
		Scene* GetScene();
		TransformComponent* GetTransform();
		ObjectComponent* GetObjectComponent();
		ECS::Entity GetEntity()const { return mObjectEntity; }

	private:
		Cjing3D::ECS::Entity mObjectEntity = Cjing3D::ECS::INVALID_ENTITY;
		Cjing3D::StringID mName;
		Scene* mScene = nullptr;
	};

	using GameObjectPtr = std::shared_ptr<GameObject>;
}