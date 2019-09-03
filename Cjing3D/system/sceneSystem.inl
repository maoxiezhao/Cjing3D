#pragma once

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