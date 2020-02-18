#pragma once

#include "system\component\componentInclude.h"

namespace Cjing3D {

	class ObjectComponent : public Component
	{
	public:
		enum ObjectType
		{
			OjbectType_Renderable
		};

		ObjectComponent() : Component(ComponentType_Ojbect) {}

		inline ObjectType GetObjectType() { return mObjectType; }
		inline RenderableType GetRenderableType() { return mRenderableType; }
		inline void SetCastingShadow(bool isCastingShadow) { mIsCastingShadow = isCastingShadow; }

		virtual void Serialize(Archive& archive, U32 seed = 0);
		virtual void Unserialize(Archive& archive)const;

	public:
		ECS::Entity mMeshID = ECS::INVALID_ENTITY;
		F32x4 mColor = F32x4(1.0f, 1.0f, 1.0f, 1.0f);
		ObjectType mObjectType = OjbectType_Renderable;
		RenderableType mRenderableType = RenderableType_Opaque;
		bool mIsCastingShadow = false;

		F32x3 mCenter = F32x3(0.0f, 0.0f, 0.0f);			// assign in system'update
	};
}

