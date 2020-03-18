#include "genericType.h"

namespace Cjing3D
{
	void AABBComponent::Serialize(Archive& archive, U32 seed)
	{
		mAABB.Serialize(archive, seed);
	}
	void AABBComponent::Unserialize(Archive& archive) const
	{
		mAABB.Unserialize(archive);
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

	void NameComponent::Serialize(Archive& archive, U32 seed)
	{
		mName.Serialize(archive, seed);
	}

	void NameComponent::Unserialize(Archive& archive) const
	{
		mName.Unserialize(archive);
	}
}