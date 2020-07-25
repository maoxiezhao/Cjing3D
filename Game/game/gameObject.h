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

		void LoadModel(const std::string& path);

		GameObject Duplicate();
		Scene* GetScene();

	private:
		TransformComponent* GetTransform();
		ObjectComponent* GetObject();

	private:
		Cjing3D::ECS::Entity mObjectEntity = Cjing3D::ECS::INVALID_ENTITY;
		Cjing3D::StringID mName;
		Scene* mScene = nullptr;
	};
}