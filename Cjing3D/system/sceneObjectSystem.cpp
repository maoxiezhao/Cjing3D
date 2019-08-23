#include "system\sceneSystem.h"

namespace Cjing3D
{
	void UpdateSceneObjectSystem(Scene& scene)
	{
		ECS::ComponentManager<ObjectComponent>& objects = scene.mObjects;
		ECS::ComponentManager<AABB>& objectAABBs = scene.mObjectAABBs;
		ECS::ComponentManager<MeshComponent>& meshes = scene.mMeshes;
		ECS::ComponentManager<TransformComponent>& transforms = scene.mTransforms;
		ECS::ComponentManager<MaterialComponent>& materials = scene.mMaterials;

		AABB sceneAABB;
		for (size_t i = 0; i < objects.GetCount(); i++)
		{
			std::shared_ptr<ObjectComponent> object = objects[i];
			std::shared_ptr<AABB> aabb = objectAABBs[i];

			if (object == nullptr || aabb == nullptr) {
				continue;
			}

			// 遍历所有的object根据对应的transform，来更新AABB, 更新object的位置

			if (object->mMeshID != ECS::INVALID_ENTITY)
			{
				Entity entity = objects.GetEntityByIndex(i);
				std::shared_ptr<MeshComponent> mesh = meshes.GetComponent(object->mMeshID);

				const auto transformIndex = transforms.GetEntityIndex(entity);
				const auto transform = transforms[transformIndex];
				if (mesh != nullptr)
				{
					XMFLOAT4X4 worldMatrix = transform->GetWorldTransform();
					auto meshAABB = mesh->mAABB.GetByTransforming(worldMatrix);
					aabb->CopyFromOther(meshAABB);

					XMFLOAT4X4 boxMatrix;
					XMStoreFloat4x4(&boxMatrix, aabb->GetBoxMatrix());

					// update center pos
					XMFLOAT3 centerPos = *((XMFLOAT3*)&boxMatrix._41);
					object->mCenter = XMStore<F32x3>(XMLoadFloat3(&centerPos));

					for (auto& subset : mesh->mSubsets)
					{
						auto material = materials.GetComponent(subset.mMaterialID);
						if (material != nullptr)
						{
							object->SetCastingShadow(material->IsCastingShadow());
						}
					}

					sceneAABB = AABB::Union(sceneAABB, *aabb);
				}
			}
		}

		scene.mSceneAABB.CopyFromOther(sceneAABB);
	}
}
