#pragma once

#include "utils\intersectable.h"
#include "world\component\componentInclude.h"

namespace Cjing3D {

	enum CameraProjectionType
	{
		CameraProjectionType_Perspective,
		CameraProjectionType_Orthographic
	};

	class CameraComponent : public Component
	{
	public:
		CameraComponent();
		virtual ~CameraComponent();

		virtual void Initialize();
		virtual void Uninitialize();
		virtual void Update();

		inline XMMATRIX GetViewMatrix() { return XMLoadFloat4x4(&mView); };
		inline XMMATRIX GetProjectionMatrix() { return XMLoadFloat4x4(&mProjection); };
		inline XMMATRIX GetViewProjectionMatrix() { return XMLoadFloat4x4(&mViewProjection); };

		F32 GetNearPlane() { return mNearPlane; }
		F32 GetFarPlane() { return mFarPlane; }
		Frustum GetFrustum()const { return mFrustum; }
		F32x3 GetCameraPos()const { return mEye; }

		void SetupPerspective(F32 width, F32 height, F32 nearPlane, F32 farPlane, F32 fov = XM_PI/2.0f);
		void SetCameraStatus(F32x3 eye, F32x3 at, F32x3 up = { 0.0f, 1.0f, 0.0f });
		void SetDirty(bool isDirty) { mIsDirty = isDirty; }
		bool IsDirty()const { return mIsDirty; }

	private:
		void ComputeProjection();

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
		XMFLOAT4X4 mView, mProjection, mViewProjection;
	};

	using CameraPtr = std::shared_ptr<CameraComponent>;
}