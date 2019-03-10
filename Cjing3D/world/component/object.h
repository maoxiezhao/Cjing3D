#pragma once

#include "world\component\componentInclude.h"

namespace Cjing3D {

	class ObjectComponent : public Component
	{
	public:
		ObjectComponent() : Component(ComponentType_Ojbect) {}

		enum ObjectType
		{
			OjbectType_Renderable
		};

		ECS::Entity mMeshID = ECS::INVALID_ENTITY;
		F32x3 mCenter = F32x3(0.0f, 0.0f, 0.0f);
		F32x4 mColor = F32x4(1.0f, 1.0f, 1.0f, 1.0f);
		ObjectType mObjectType = OjbectType_Renderable;

		RenderableType mRenderableType = RenderableType_Opaque;
		bool mIsCastingShadow = false;

		inline ObjectType GetObjectType() { return mObjectType; }
		inline RenderableType GetRenderableType() { return mRenderableType; }
		inline void SetCastingShadow(bool isCastingShadow) { mIsCastingShadow = isCastingShadow; }
	};
}

