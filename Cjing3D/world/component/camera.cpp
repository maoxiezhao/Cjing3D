#include "camera.h"

namespace Cjing3D
{
	Camera::Camera(SystemContext& systemContext):
		Component(systemContext),
		mIsDirty(true)
	{
		mNearPlane = 0.001f;
		mFarPlane = 800.0f;
		mFov = XM_PI / 3.0f;
		mWidth = 1024;
		mHeight = 768;

		mEye = { 0.0f, 0.0f, -50.0f };
		mAt =  { 0.0f, 0.0f, 1.0f };
		mUp =  { 0.0f, 1.0f, 0.0f };

		mProjectionType = CameraProjectionType_Perspective;

		ComputeProjection();
		Update();
	}

	Camera::~Camera()
	{
	}

	void Camera::Initialize()
	{
	}

	void Camera::Uninitialize()
	{
	}

	void Camera::Update()
	{
		if (mIsDirty == true)
		{
			mIsDirty = false;

			// get view
			XMMATRIX view = XMMatrixLookToLH(
				XMLoad(mEye),
				XMLoad(mAt),
				XMLoad(mUp)
			);
			XMStoreFloat4x4(&mView, view);

			XMMATRIX vp = XMMatrixMultiply(view, XMLoadFloat4x4(&mProjection));
			XMStoreFloat4x4(&mViewProjection, vp);

			mFrustum.SetupFrustum(mView, mProjection, mFarPlane);
		}
	}

	void Camera::SetupPerspective(F32 width, F32 height, F32 nearPlane, F32 farPlane, F32 fov)
	{
		mWidth = width;
		mHeight = height;
		mNearPlane = nearPlane;
		mFarPlane = farPlane;
		mFov = fov;
		mProjectionType = CameraProjectionType_Perspective;

		mIsDirty = true;

		ComputeProjection();
		Update();
	}

	// 仅在创建相机时计算projection
	void Camera::ComputeProjection()
	{
		XMStoreFloat4x4(&mProjection, XMMatrixPerspectiveFovLH(mFov, mWidth / mHeight, mNearPlane, mFarPlane));
	}
}