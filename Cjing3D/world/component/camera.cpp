#include "camera.h"

namespace Cjing3D
{
	CameraComponent::CameraComponent():
		Component(ComponentType_Camera),
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

	CameraComponent::~CameraComponent()
	{
	}

	void CameraComponent::Initialize()
	{
	}

	void CameraComponent::Uninitialize()
	{
	}

	void CameraComponent::Update()
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

	void CameraComponent::SetupPerspective(F32 width, F32 height, F32 nearPlane, F32 farPlane, F32 fov)
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
	void CameraComponent::ComputeProjection()
	{
		XMStoreFloat4x4(&mProjection, XMMatrixPerspectiveFovLH(mFov, mWidth / mHeight, mNearPlane, mFarPlane));
	}
}