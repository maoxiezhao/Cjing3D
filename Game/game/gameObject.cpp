#include "gameObject.h"

namespace CjingGame
{
	GameObject::GameObject()
	{
	}

	GameObject::GameObject(const Cjing3D::StringID& name) :
		mName(name)
	{
	}

	GameObject::~GameObject()
	{
		RemoveFromScene();
	}

	void GameObject::LoadFromScene(Scene& scene)
	{
		if (IsOnScene()) {
			RemoveFromScene();
		}

		mScene = &scene;
		mObjectEntity = mScene->CreateEntityObject(mName.GetString());
	}

	void GameObject::RemoveFromScene()
	{
		if (IsOnScene()) 
		{
			mScene->RemoveEntity(mObjectEntity);
			mScene = nullptr;
		}
	}

	bool GameObject::IsOnScene() const
	{
		return mObjectEntity != Cjing3D::ECS::INVALID_ENTITY && mScene != nullptr;
	}

	void GameObject::SetPosition(const F32x3& pos)
	{
		auto transform = GetTransform();
		if (transform != nullptr) {
			transform->SetTranslationLocal(XMConvert(pos));
		}
	}

	void GameObject::SetRotation(const F32x3& rotation)
	{
		auto transform = GetTransform();
		if (transform != nullptr) {
			transform->SetRotateFromRollPitchYaw(XMConvert(rotation));
		}
	}

	void GameObject::SetScale(const F32x3& scale)
	{
		auto transform = GetTransform();
		if (transform != nullptr) {
			transform->SetScaleLocal(XMConvert(scale));
		}
	}

	void GameObject::SetVisible(bool visible)
	{
		auto object = GetObjectComponent();
		if (object != nullptr) {
			object->SetRenderable(visible);
		}
	}

	void GameObject::SetRenderType(RenderableType type)
	{
		auto object = GetObjectComponent();
		if (object != nullptr) {
			object->SetRenderableType(type);
		}
	}

	void GameObject::SetMeshFromModel(const std::string& path)
	{
		auto object = GetObjectComponent();
		if (object == nullptr) {
			return;
		}

		Scene newScene;
		newScene.LoadModel(path);
		if (newScene.mMeshes.Empty()) {
			return;
		}

		object->mMeshID = newScene.mMeshes.GetEntityByIndex(0);

		for (const auto& entity : newScene.mObjects.GetEntities()) {
			newScene.RemoveEntity(entity);
		}
		mScene->Merge(newScene);
	}

	void GameObject::SetMesh(const ECS::Entity& meshEntity)
	{
		auto object = GetObjectComponent();
		if (object == nullptr) {
			return;
		}

		object->mMeshID = meshEntity;
	}

	GameObject GameObject::Duplicate()
	{
		if (!IsOnScene()) {
			return GameObject();
		}

		GameObject newObject = *this;
		newObject.mObjectEntity = mScene->DuplicateEntity(mObjectEntity);
		return newObject;
	}

	Scene* GameObject::GetScene()
	{
		return mScene;
	}

	TransformComponent* GameObject::GetTransform()
	{
		if (!IsOnScene()) {
			return nullptr;
		}
		
		return mScene->mTransforms.GetComponent(mObjectEntity);
	}

	ObjectComponent* GameObject::GetObjectComponent()
	{
		if (!IsOnScene()) {
			return nullptr;
		}

		return mScene->mObjects.GetComponent(mObjectEntity);
	}
}