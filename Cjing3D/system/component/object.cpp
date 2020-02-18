#include "object.h"

void Cjing3D::ObjectComponent::Serialize(Archive& archive, U32 seed)
{
	mMeshID = SerializeEntity(archive, seed);
	archive >> mColor;
	archive >> mObjectType;
	archive >> mRenderableType;
}

void Cjing3D::ObjectComponent::Unserialize(Archive& archive) const
{
	archive << mMeshID;
	archive << mColor;
	archive << mObjectType;
	archive << mRenderableType;
}
