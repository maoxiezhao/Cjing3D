#include "intersectable.h"

namespace Cjing3D
{
	Frustum::Frustum()
	{
	}

	Frustum::~Frustum()
	{
	}

	void Frustum::SetupFrustum(const XMFLOAT4X4 & view, const XMFLOAT4X4 & projection, float screenDepth)
	{
	}

	void Frustum::SetupFrustum(XMMATRIX transform)
	{
		// 根据最终是否在视口范围内创建视锥体，所有的平面法线
		// 方向都是朝内的
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
		// 检测AABB在每个平面的法线方向的最大点与法线的点积
		// 小于0则表示极点在平面外
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

	void AABB::CopyFromOther(const AABB & aabb)
	{
		mMin = aabb.mMin;
		mMax = aabb.mMax;
	}
}