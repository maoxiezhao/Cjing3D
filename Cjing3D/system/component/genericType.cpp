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
	}

	void HierarchyComponent::Unserialize(Archive& archive) const
	{
		archive << mParent;
	}

	void NameComponent::Serialize(Archive& archive, U32 seed)
	{
		mName.Serialize(archive, seed);
	}

	void NameComponent::Unserialize(Archive& archive) const
	{
		mName.Unserialize(archive);
	}

	void LayerComponent::Serialize(Archive& archive, U32 seed)
	{
		archive >> mLayerMask;
	}

	void LayerComponent::Unserialize(Archive& archive) const
	{
		archive << mLayerMask;
	}
}