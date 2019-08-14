#pragma once

#include "system\component\componentInclude.h"

namespace Cjing3D {

	class TransformComponent : public Component
	{
	public:
		TransformComponent();

		void Update();
		void Clear();

		void Translate(const XMFLOAT3& value);
		void RotateRollPitchYaw(const XMFLOAT3& value);
		void Rotate(const XMFLOAT4& quaternion);
		void Scale(const XMFLOAT3& value);

		inline void SetDirty(bool isDirty) { mIsDirty = isDirty; }
		inline bool IsDirty()const { return mIsDirty; }
		inline XMFLOAT4X4 GetWorldTransform() const { return mWorldTransform; }

	private:
		XMFLOAT3 mTranslationLocal = XMFLOAT3(0.0f, 0.0f, 0.0f);
		XMFLOAT4 mRotationLocal = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
		XMFLOAT3 mScaleLocal = XMFLOAT3(1.0f, 1.0f, 1.0f);
		XMFLOAT4X4 mWorldTransform = IDENTITYMATRIX;

		bool mIsDirty = true;
	};
}