#pragma once

#include "system\component\componentInclude.h"

namespace Cjing3D {

	LUA_BINDER_REGISTER_CLASS
	class TransformComponent : public Component
	{
	public:
		TransformComponent();

		LUA_BINDER_REGISTER_CLASS_SET_NAME(Transform)
		LUA_BINDER_REGISTER_CLASS_METHOD_FUNCTION
		void Update();

		LUA_BINDER_REGISTER_CLASS_METHOD_FUNCTION
		void Clear();

		LUA_BINDER_REGISTER_CLASS_METHOD_FUNCTION
		void Translate(const XMFLOAT3& value);

		LUA_BINDER_REGISTER_CLASS_METHOD_FUNCTION
		void RotateRollPitchYaw(const XMFLOAT3& value);

		LUA_BINDER_REGISTER_CLASS_METHOD_FUNCTION
		void Scale(const XMFLOAT3& value);

		void Rotate(const XMFLOAT4& quaternion);

		LUA_BINDER_REGISTER_CLASS_METHOD_FUNCTION
		inline XMFLOAT4 GetRotationLocal()const { return mRotationLocal; }

		LUA_BINDER_REGISTER_CLASS_METHOD_FUNCTION
		inline XMFLOAT3 GetTranslationLocal()const { return mTranslationLocal; }

		LUA_BINDER_REGISTER_CLASS_METHOD_FUNCTION
		inline XMFLOAT3 GetScaleLocal()const { return mScaleLocal; }

		void SetRotateFromRollPitchYaw(const XMFLOAT3& value);
		void SetRotationLocal(const XMFLOAT4& value);
		void SetTranslationLocal(const XMFLOAT3& value);
		void SetScaleLocal(const XMFLOAT3& value);
		void SetWorldTransform(const XMFLOAT4X4& world);

		void ApplyTransform();
		void UpdateFromParent(TransformComponent& parent, const XMMATRIX& parentBindInverseWorld);

		inline void SetDirty(bool isDirty) { mIsDirty = isDirty; }
		inline bool IsDirty()const { return mIsDirty; }
		inline XMFLOAT4X4 GetWorldTransform() const { return mWorldTransform; }

		virtual void Serialize(Archive& archive, U32 seed = 0);
		virtual void Unserialize(Archive& archive)const;

	private:
		XMFLOAT3 mTranslationLocal = XMFLOAT3(0.0f, 0.0f, 0.0f);
		XMFLOAT4 mRotationLocal = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
		XMFLOAT3 mScaleLocal = XMFLOAT3(1.0f, 1.0f, 1.0f);
		XMFLOAT4X4 mWorldTransform = IDENTITYMATRIX;

		bool mIsDirty = true;
	};
}