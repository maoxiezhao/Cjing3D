#include "intersectable.h"

namespace Cjing3D
{
	Frustum::Frustum()
	{
	}

	Frustum::~Frustum()
	{
	}

	void Frustum::SetupFrustum(XMMATRIX transform)
	{
		// ���������Ƿ����ӿڷ�Χ�ڴ�����׶�壬���е�ƽ�淨��
		// �����ǳ��ڵ�
		const auto t = XMMatrixTranspose(transform);
		// p' = p T = (pC1, pC2, pC3, pC4)

		// (-1 < x'/w)  => (-w' <= x)'
		// 0 <= x' + w' => p(C1 + C4)
		mleftPlane = XMPlaneNormalize(t.r[0] + t.r[3]);
		mRightPlane = XMPlaneNormalize(t.r[3] - t.r[0]);
		mTopPlane = XMPlaneNormalize(t.r[3] - t.r[1]);
		mBottomPlane = XMPlaneNormalize(t.r[3] + t.r[1]);
		mNearPlane = XMPlaneNormalize(t.r[2]);
		mFarPlane = XMPlaneNormalize(t.r[3] - t.r[2]);
	}

	bool Frustum::Overlaps(const AABB & aabb) const
	{
		// ���AABB��ÿ��ƽ��ķ��߷���������뷨�ߵĵ��
		// С��0���ʾ������ƽ����
		for (int i = 0; i < std::size(mPlanes); i++)
		{
			const auto p = aabb.GetMaxPointAlongNormal(mPlanes[i]);
			const auto result = XMPlaneDotCoord(mPlanes[i], p);
			if (XMVectorGetX(result) < 0.0f)
			{
				return false;
			}
		}
		return true;
	}

	AABB AABB::GetByTransforming(const XMFLOAT4X4 & mat) const
	{
		return AABB();
	}

	XMMATRIX AABB::GetBoxMatrix() const
	{
		F32x3 ext = XMStore<F32x3>(GetRadius());
		XMMATRIX sca = XMMatrixScaling(ext[0], ext[1], ext[2]);
		F32x3 pos = XMStore<F32x3>(GetCenter());
		XMMATRIX trans = XMMatrixTranslation(pos[0], pos[1], pos[2]);

		return sca * trans;
	}

	void AABB::CopyFromOther(const AABB & aabb)
	{
		mMin = aabb.mMin;
		mMax = aabb.mMax;
	}
}