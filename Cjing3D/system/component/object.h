#pragma once

#include "system\component\componentInclude.h"

namespace Cjing3D {

	struct RenderInstance;

	LUA_BINDER_REGISTER_CLASS
	class ObjectComponent : public Component
	{
	public:
		enum ObjectType
		{
			OjbectType_EMPTY = 0,
			OjbectType_Renderable = 1
		};

		ObjectComponent() : Component(ComponentType_Ojbect) {}

		inline ObjectType GetObjectType() const { return static_cast<ObjectType>(mObjectType); }
		inline RenderableType GetRenderableType()const { return mRenderableType; }
		inline void SetRenderableType(RenderableType type) { mRenderableType = type; }
		
		LUA_BINDER_REGISTER_CLASS_METHOD_FUNCTION
		inline bool IsRenderable()const { 
			return  GetObjectType() == OjbectType_Renderable && mMeshID != INVALID_ENTITY;
		}

		LUA_BINDER_REGISTER_CLASS_METHOD_FUNCTION
		inline void SetRenderable(bool value) { 
			mObjectType = value ? mObjectType | OjbectType_Renderable : mObjectType & ~OjbectType_Renderable;
		}

		inline void SetCastShadow(bool isCastShadow) { mIsCastShadow = isCastShadow; }
		inline bool IsCastShadow()const { return mIsCastShadow; }
		inline void SetIsImpostor(bool isImpostor) { mIsImpostor = isImpostor; }
		inline bool IsImpostor()const { return mIsImpostor; }

		virtual void Serialize(Archive& archive, U32 seed = 0);
		virtual void Unserialize(Archive& archive)const;

	public:
		ECS::Entity mMeshID = ECS::INVALID_ENTITY;
		F32x4 mColor = F32x4(1.0f, 1.0f, 1.0f, 1.0f);
		U32 mObjectType = OjbectType_Renderable;
		RenderableType mRenderableType = RenderableType_Opaque;
		U32 mShadowCascadeMask = 0;	// ≤„º∂“ı”∞mask
		bool mIsCastShadow = false;
		bool mIsImpostor = false;
		F32x3 mCenter = F32x3(0.0f, 0.0f, 0.0f);			// assign in system'update
	
		using ProcessInstanceFunc = std::function<void(RenderInstance&)>;
		ProcessInstanceFunc RenderInstanceHandler = nullptr;
	};
}

