#pragma once

#include "world\component\component.h"
#include "utils\intersectable.h"

namespace Cjing3D {

	enum CameraProjectionType
	{
		CameraProjectionType_Perspective,
		CameraProjectionType_Orthographic
	};

	class Camera : public Component
	{
	public:
		Camera(SystemContext& systemContext);
		virtual ~Camera();

		virtual void Initialize();
		virtual void Uninitialize();
		virtual void Update();

		inline XMMATRIX GetViewMatrix() { return XMLoadFloat4x4(&mView); };
		inline XMMATRIX GetProjectionMatrix() { return XMLoadFloat4x4(&mProjection); };
		inline XMMATRIX GetViewProjectionMatrix() { return XMLoadFloat4x4(&mViewProjection); };

		F32 GetNearPlane() { return mNearPlane; }
		F32 GetFarPlane() { return mFarPlane; }
		Frustum GetFrustum()const { return mFrustum; }

		void SetupPerspective(F32 width, F32 height, F32 nearPlane, F32 farPlane, F32 fov = XM_PI/2.0f);

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

	using CameraPtr = std::shared_ptr<Camera>;
}