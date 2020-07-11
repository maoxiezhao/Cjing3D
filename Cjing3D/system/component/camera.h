#pragma once

#include "utils\intersectable.h"
#include "system\component\componentInclude.h"
#include "system\component\transform.h"

namespace Cjing3D {

	enum CameraProjectionType
	{
		CameraProjectionType_Perspective,
		CameraProjectionType_Orthographic
	};

	LUA_BINDER_REGISTER_CLASS
	class CameraComponent : public Component
	{
	public:
		CameraComponent();
		virtual ~CameraComponent();

		LUA_BINDER_REGISTER_CLASS_METHOD_FUNCTION
		virtual void Update();

		LUA_BINDER_REGISTER_CLASS_METHOD_FUNCTION
		void Transform(const TransformComponent& transform);

		inline XMMATRIX GetViewMatrix()const { return XMLoadFloat4x4(&mView); };
		inline XMMATRIX GetProjectionMatrix()const { return XMLoadFloat4x4(&mProjection); };
		inline XMMATRIX GetViewProjectionMatrix()const { return XMLoadFloat4x4(&mViewProjection); };
		inline XMMATRIX GetInvViewMatrix()const { return XMLoadFloat4x4(&mInvView); }
		inline XMMATRIX GetInvProjectionMatrix()const { return XMLoadFloat4x4(&mInvProjection); }
		inline XMMATRIX GetInvViewProjectionMatrix()const { return XMLoadFloat4x4(&mInvViewProjection); }

		F32 GetNearPlane() { return mNearPlane; }
		F32 GetFarPlane() { return mFarPlane; }
		Frustum GetFrustum()const { return mFrustum; }
		F32x3 GetCameraPos()const { return mEye; }
		F32x3 GetCameraUP()const { return mUp; }
		F32x3 GetCameraFront()const { return mAt; }

		void SetupPerspective(F32 width, F32 height, F32 nearPlane, F32 farPlane, F32 fov = XM_PI/3.0f);
		void SetCameraStatus(F32x3 eye, F32x3 at, F32x3 up = { 0.0f, 1.0f, 0.0f });
		void SetDirty(bool isDirty) { mIsDirty = isDirty; }
		bool IsDirty()const { return mIsDirty; }

	private:
		CameraProjectionType mProjectionType;
		bool mIsDirty;
		Frustum mFrustum;

		F32 mNearPlane = 0.001f;
		F32 mFarPlane = 800.0f;
		F32 mFov = XM_PI / 3.0f;
		F32 mWidth = 0.0f;
		F32 mHeight = 0.0f;

		F32x3 mEye, mAt, mUp;
		XMFLOAT4X4 mView;
		XMFLOAT4X4 mProjection;
		XMFLOAT4X4 mViewProjection;
		XMFLOAT4X4 mInvView;
		XMFLOAT4X4 mInvProjection;
		XMFLOAT4X4 mInvViewProjection;
	};
}