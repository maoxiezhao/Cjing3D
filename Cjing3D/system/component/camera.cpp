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

		Update();
	}

	CameraComponent::~CameraComponent()
	{
	}

	void CameraComponent::Update()
	{
		if (mIsDirty == true) {
			mIsDirty = false;

			// reverse zbuffer
			XMStoreFloat4x4(&mProjection, XMMatrixPerspectiveFovLH(mFov, mWidth / mHeight, mFarPlane, mNearPlane));

			XMMATRIX view = XMMatrixLookToLH(
				XMLoad(mEye),
				XMLoad(mAt),
				XMLoad(mUp)
			);
			XMStoreFloat4x4(&mView, view);

			XMMATRIX invP = XMMatrixInverse(nullptr, XMLoadFloat4x4(&mProjection));
			XMStoreFloat4x4(&mInvProjection, invP);

			XMMATRIX vp = XMMatrixMultiply(view, XMLoadFloat4x4(&mProjection));
			XMStoreFloat4x4(&mViewProjection, vp);

			XMMATRIX invVP = XMMatrixInverse(nullptr, vp);
			XMStoreFloat4x4(&mInvViewProjection, invVP);

			//mFrustum.SetupFrustum(mView, mProjection, mFarPlane);
			mFrustum.SetupFrustum(vp);
		}
	}

	void CameraComponent::Transform(const TransformComponent & transform)
	{
		auto worldTransform = transform.GetWorldTransform();
		XMVECTOR scale, rotation, translate;
		XMMatrixDecompose(&scale, &rotation, &translate, XMLoadFloat4x4(&worldTransform));

		XMVECTOR eye = translate;
		XMVECTOR at = XMVectorSet(0, 0, 1, 0);
		XMVECTOR up = XMVectorSet(0, 1, 0, 0);

		XMMATRIX rotMat = XMMatrixRotationQuaternion(rotation);
		at = XMVector3TransformNormal(at, rotMat);
		up = XMVector3TransformNormal(up, rotMat);

		mEye = XMStore<F32x3>(eye);
		mAt = XMStore<F32x3>(at);
		mUp = XMStore<F32x3>(up);

		mIsDirty = true;
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

		Update();
	}

	void CameraComponent::SetCameraStatus(F32x3 eye, F32x3 at, F32x3 up)
	{
		mEye = eye;
		mAt = at;
		mUp = up;
		mIsDirty = true;
	}
}