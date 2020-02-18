#include "sceneSystem.h"
#include "helper\random.h"

namespace Cjing3D
{
	using namespace ECS;

	void Scene::Serialize(Archive& archive)
	{
		U32 entityCount = 0;
		archive >> entityCount;

		if (entityCount <= 0) {
			return;
		}

		// 依次序列化所有的componentManager
		U32 seed = GENERATE_RANDOM_ID;

		mNames.Serialize(archive, seed);
		mTransforms.Serialize(archive, seed);
		mHierarchies.Serialize(archive, seed);
		mMaterials.Serialize(archive, seed);
		mMeshes.Serialize(archive, seed);
		mObjects.Serialize(archive, seed);
		mObjectAABBs.Serialize(archive, seed);
		mLights.Serialize(archive, seed);
		mLightAABBs.Serialize(archive, seed);

		// reset name mapping
		for (auto entity : mNames.GetEntities())
		{
			auto entityName = mNames.GetComponent(entity);
			mNameEntityMap[*entityName] = entity;
		}
	}

	void Scene::Unserialize(Archive& archive) const
	{
		U32 entityCount = GetEntityCount();
		archive << entityCount;

		if (entityCount <= 0) {
			return;
		}

		mNames.Unserialize(archive);
		mTransforms.Unserialize(archive);
		mHierarchies.Unserialize(archive);
		mMaterials.Unserialize(archive);
		mMeshes.Unserialize(archive);
		mObjects.Unserialize(archive);
		mObjectAABBs.Unserialize(archive);
		mLights.Unserialize(archive);
		mLightAABBs.Unserialize(archive);
	}

	void HierarchyComponent::Serialize(Archive& archive, U32 seed)
	{
		mParent = SerializeEntity(archive, seed);

		XMFLOAT4X4 parentBindInverseWorld;
		archive >> parentBindInverseWorld;

		mParentBindInverseWorld = XMLoadFloat4x4(&parentBindInverseWorld);
	}

	void HierarchyComponent::Unserialize(Archive& archive) const
	{
		archive << mParent;

		XMFLOAT4X4 parentBindInverseWorld;
		XMStoreFloat4x4(&parentBindInverseWorld, mParentBindInverseWorld);
		archive << parentBindInverseWorld;
	}
}