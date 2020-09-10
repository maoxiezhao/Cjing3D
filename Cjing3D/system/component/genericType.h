#pragma once

#include "system\component\componentInclude.h"

namespace Cjing3D {

	class AABBComponent : public Component
	{
	public:
		AABBComponent() : Component(ComponentType_AABB) {}

		virtual void Serialize(Archive& archive, U32 seed = 0);
		virtual void Unserialize(Archive& archive)const;

		void SetAABB(const AABB& aabb) { mAABB = aabb; }
		const AABB GetAABB()const { return mAABB; }
		AABB& GetAABB() { return mAABB; }

		AABB mAABB;
	};

	class NameComponent : public Component
	{
	public:
		NameComponent() : Component(ComponentType_Name) {}

		virtual void Serialize(Archive& archive, U32 seed = 0);
		virtual void Unserialize(Archive& archive)const;

		void SetName(const std::string& name) { mName = name; }
		void SetName(const StringID& name) { mName = name; }
		const StringID GetName()const { return mName; }
		StringID& GetName() { return mName; }
		const std::string GetString()const { return mName.GetString(); }

		StringID mName;
	};

	class LayerComponent : public Component
	{
	public:
		LayerComponent() : Component(ComponentType_Layer) {}

		static const U32 DefaultLayerMask = 0xFFFFFFFF;
		U32 mLayerMask = DefaultLayerMask;

		inline void SetLayerMask(U32 mask) { mLayerMask = mask; }
		inline U32 GetLayerMask()const { return mLayerMask; }

		virtual void Serialize(Archive& archive, U32 seed = 0);
		virtual void Unserialize(Archive& archive)const;

	};

	//  Hierarchy������֯Transform֮��Ĳ㼶��ϵ
	class HierarchyComponent : public Component
	{
	public:
		HierarchyComponent() : Component(ComponentType_HierarchyComponent) {}

		// parent entity
		ECS::Entity mParent = ECS::INVALID_ENTITY;
		U32 mChildBindLayerMask = LayerComponent::DefaultLayerMask;

		virtual void Serialize(Archive& archive, U32 seed = 0);
		virtual void Unserialize(Archive& archive)const;
	};
}

