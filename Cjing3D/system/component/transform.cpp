#include "transform.h"

namespace Cjing3D
{
	TransformComponent::TransformComponent():
		Component(ComponentType_Transform)
	{
	}

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

	// 基于当前位置继续位移
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

	void TransformComponent::SetRotateFromRollPitchYaw(const XMFLOAT3& value)
	{
		XMFLOAT4 indentityQuat(0.0f, 0.0f, 0.0f, 1.0f);
		XMVECTOR quat = XMLoadFloat4(&indentityQuat);
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

	void TransformComponent::SetRotationLocal(const XMFLOAT4& value)
	{
		mRotationLocal = value;
		mIsDirty = true;
	}

	void TransformComponent::SetTranslationLocal(const XMFLOAT3& value)
	{
		mTranslationLocal = value;
		mIsDirty = true;
	}

	void TransformComponent::SetScaleLocal(const XMFLOAT3& value)
	{
		mScaleLocal = value;
		mIsDirty = true;
	}

	void TransformComponent::SetWorldTransform(const XMFLOAT4X4& world)
	{
		mWorldTransform = world;
		mIsDirty = true;
	}

	void TransformComponent::ApplyTransform()
	{
		// 以当前世界坐标位置给local变量赋值
		XMVECTOR s, r, t;
		XMMatrixDecompose(&s, &r, &t, XMLoadFloat4x4(&mWorldTransform));
		XMStoreFloat3(&mScaleLocal, s);
		XMStoreFloat4(&mRotationLocal, r);
		XMStoreFloat3(&mTranslationLocal, t);

		mIsDirty = true;
	}

	void TransformComponent::UpdateFromParent(TransformComponent& parent, const XMMATRIX& parentBindInverseWorld)
	{
		// 在transform层级关系中，mWorldTransform只表示最终的世界坐标系下变换
		// 所以child节点需要先乘以绑定时的父节点变换的逆矩阵，来恢复绑定前的状态，再乘以新的parent transform

		// parent正常情况下非dirty, 但某些情况下被其他系统手动设置为dirty
		if (parent.IsDirty())
		{
			SetDirty(true);

			XMMATRIX childWorld = XMLoadFloat4x4(&mWorldTransform);
			XMMATRIX parentWorld = XMLoadFloat4x4(&parent.GetWorldTransform());

			childWorld = childWorld * parentBindInverseWorld * parentWorld;
			XMStoreFloat4x4(&mWorldTransform, childWorld);
		}
		else
		{
			XMVECTOR s = XMLoadFloat3(&mScaleLocal);
			XMVECTOR r = XMLoadFloat4(&mRotationLocal);
			XMVECTOR t = XMLoadFloat3(&mTranslationLocal);
			XMMATRIX childWorld = XMMatrixScalingFromVector(s) * XMMatrixRotationQuaternion(r) * XMMatrixTranslationFromVector(t);
			XMMATRIX parentWorld = XMLoadFloat4x4(&parent.GetWorldTransform());
		
			childWorld = childWorld * parentBindInverseWorld * parentWorld;
			XMStoreFloat4x4(&mWorldTransform, childWorld);
		}
	}

	void TransformComponent::Serialize(Archive& archive, U32 seed)
	{
		archive >> mTranslationLocal;
		archive >> mRotationLocal;
		archive >> mScaleLocal;

		SetDirty(true);
		Update();
	}

	void TransformComponent::Unserialize(Archive& archive) const
	{
		archive << mTranslationLocal;
		archive << mRotationLocal;
		archive << mScaleLocal;
	}

	void TransformComponent::Scale(const XMFLOAT3 & value)
	{
		mScaleLocal.x *= value.x;
		mScaleLocal.y *= value.y;
		mScaleLocal.z *= value.z;

		mIsDirty = true;
	}
}