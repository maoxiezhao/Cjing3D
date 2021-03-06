#include "system\sceneSystem.h"

namespace Cjing3D
{
	void SceneSystem::UpdateSceneObjectSystem(Scene& scene)
	{
		ECS::ComponentManager<ObjectComponent>& objects = scene.mObjects;
		ECS::ComponentManager<AABBComponent>& objectAABBs = scene.mObjectAABBs;
		ECS::ComponentManager<MeshComponent>& meshes = scene.mMeshes;
		ECS::ComponentManager<TransformComponent>& transforms = scene.mTransforms;
		ECS::ComponentManager<MaterialComponent>& materials = scene.mMaterials;

		AABB sceneAABB;
		for (size_t i = 0; i < objects.GetCount(); i++)
		{
			ObjectComponent* object = objects[i];
			AABBComponent* aabbComponent = objectAABBs[i];

			if (object == nullptr || aabbComponent == nullptr) {
				continue;
			}

			// init default status
			object->SetCastShadow(false);

			// 遍历所有的object根据对应的transform，来更新AABB, 更新object的位置
			if (object->mMeshID != ECS::INVALID_ENTITY)
			{
				Entity entity = objects.GetEntityByIndex(i);
				MeshComponent* mesh = meshes.GetComponent(object->mMeshID);

				const auto transformIndex = transforms.GetEntityIndex(entity);
				const auto transform = transforms[transformIndex];
				if (mesh != nullptr)
				{
					AABB& aabb = aabbComponent->GetAABB();
					if (!mesh->IsEmpty())
					{
						XMFLOAT4X4 worldMatrix = transform->GetWorldTransform();
						auto meshAABB = mesh->mAABB.GetByTransforming(worldMatrix);
						if (mesh->IsSkinned())
						{
							meshAABB = AABB::Union(meshAABB, mesh->mAABB);
						}
						aabb.CopyFromOther(meshAABB);
					}
					else
					{
						aabb.CopyFromOther(AABB());
					}

					XMFLOAT4X4 boxMatrix;
					XMStoreFloat4x4(&boxMatrix, aabb.GetBoxMatrix());

					// update center pos
					XMFLOAT3 centerPos = *((XMFLOAT3*)&boxMatrix._41);
					object->mCenter = XMStore<F32x3>(XMLoadFloat3(&centerPos));

					for (auto& subset : mesh->mSubsets)
					{
						auto material = materials.GetComponent(subset.mMaterialID);
						if (material != nullptr)
						{
							object->SetCastShadow(material->IsCastingShadow());
							object->SetRenderableType(material->IsTransparent() ? RenderableType_Transparent : RenderableType_Opaque);
						}
					}

					sceneAABB = AABB::Union(sceneAABB, aabb);
				}
			}
		}

		scene.mSceneAABB.CopyFromOther(sceneAABB);
	}
}
