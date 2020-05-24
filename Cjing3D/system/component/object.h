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
		inline RenderableType GetRenderableType()const { return mRenderableType; }
		inline void SetRenderableType(RenderableType type) { mRenderableType = type; }
		inline bool IsRenderable()const { return  GetRenderableType() == OjbectType_Renderable;}

		inline void SetCastShadow(bool isCastShadow) { mIsCastShadow = isCastShadow; }
		inline bool IsCastShadow()const { return mIsCastShadow; }
		inline void SetIsImpostor(bool isImpostor) { mIsImpostor = isImpostor; }
		inline bool IsImpostor()const { return mIsImpostor; }

		virtual void Serialize(Archive& archive, U32 seed = 0);
		virtual void Unserialize(Archive& archive)const;

	public:
		ECS::Entity mMeshID = ECS::INVALID_ENTITY;
		F32x4 mColor = F32x4(1.0f, 1.0f, 1.0f, 1.0f);
		ObjectType mObjectType = OjbectType_Renderable;
		RenderableType mRenderableType = RenderableType_Opaque;

		bool mIsCastShadow = false;
		bool mIsImpostor = false;

		F32x3 mCenter = F32x3(0.0f, 0.0f, 0.0f);			// assign in system'update
	};
}

