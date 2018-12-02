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

	private:
		void ComputeViewMatrix();
		void ComputeProjection();

	private:
		CameraProjectionType mProjectionType;
		bool mIsDirty;
		Frustum mFrustum;

		F32 mNearPlane;
		F32 mFarPlane;
		F32 mFov;
		
		F32x3 mEye, mAt, mUp;
		XMFLOAT4X4 mView, mProjection, mViewProjection;
	};

}