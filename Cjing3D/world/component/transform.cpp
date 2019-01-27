#include "transform.h"

namespace Cjing3D
{
	void TransformComponent::Update()
	{
		if (mIsDirty == true) {
			mIsDirty = false;

			XMVECTOR s = XMLoadFloat3(&mScaleLocal);
			XMVECTOR r = XMLoadFloat4(&mRotationLocal);
			XMVECTOR t = XMLoadFloat3(&mTranslationLocal);
			XMMATRIX w = XMMatrixScalingFromVector(s) * XMMatrixRotationQuaternion(r) * XMMatrixTranslationFromVector(t);
		
			XMStoreFloat4x4(&mWorldTransform, w);
		}
	}

	void TransformComponent::Clear()
	{
		mTranslationLocal = XMFLOAT3(0.0f, 0.0f, 0.0f);
		mRotationLocal = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
		mScaleLocal = XMFLOAT3(1.0f, 1.0f, 1.0f);

		mIsDirty = true;
	}

	void TransformComponent::Translate(const XMFLOAT3 & value)
	{
		mTranslationLocal.x += value.x;
		mTranslationLocal.y += value.y;
		mTranslationLocal.z += value.z;

		mIsDirty = true;
	}

	void TransformComponent::RotateRollPitchYaw(const XMFLOAT3 & value)
	{
		XMVECTOR quat = XMLoadFloat4(&mRotationLocal);
		XMVECTOR x = XMQuaternionRotationRollPitchYaw(value.x, 0.0f, 0.0f);
		XMVECTOR y = XMQuaternionRotationRollPitchYaw(0.0f, value.y, 0.0f);
		XMVECTOR z = XMQuaternionRotationRollPitchYaw(0.0f, 0.0f, value.z);

		quat = XMQuaternionMultiply(x, quat);
		quat = XMQuaternionMultiply(quat, y);
		quat = XMQuaternionMultiply(z, quat);
		quat = XMQuaternionNormalize(quat);

		XMStoreFloat4(&mRotationLocal, quat);
		mIsDirty = true;
	}

	void TransformComponent::Rotate(const XMFLOAT4 & quaternion)
	{
		XMVECTOR quat = XMQuaternionMultiply(XMLoadFloat4(&mRotationLocal), XMLoadFloat4(&quaternion));
		quat = XMQuaternionNormalize(quat);

		XMStoreFloat4(&mRotationLocal, quat);
		mIsDirty = true;
	}

	void TransformComponent::Scale(const XMFLOAT3 & value)
	{
		mScaleLocal.x *= value.x;
		mScaleLocal.y *= value.y;
		mScaleLocal.z *= value.z;

		mIsDirty = true;
	}
}