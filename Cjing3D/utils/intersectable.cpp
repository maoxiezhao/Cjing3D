#include "intersectable.h"
#include "helper\archive.h"

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
		auto temp_proj = projection;

		// Calculate the minimum Z distance in the frustum.
		float zMinimum = -temp_proj._43 / temp_proj._33;
		float r = screenDepth / (screenDepth - zMinimum);
		temp_proj._33 = r;
		temp_proj._43 = -r * zMinimum;

		// Create the frustum matrix from the view matrix and updated projection matrix.
		XMMATRIX t = XMMatrixMultiply(XMLoadFloat4x4(&view), XMLoadFloat4x4(&temp_proj));

		// Calculate near plane of frustum.
		mleftPlane = XMPlaneNormalize(t.r[0] + t.r[3]);
		mRightPlane = XMPlaneNormalize(t.r[3] - t.r[0]);
		mTopPlane = XMPlaneNormalize(t.r[3] - t.r[1]);
		mBottomPlane = XMPlaneNormalize(t.r[3] + t.r[1]);
		mNearPlane = XMPlaneNormalize(t.r[2]);
		mFarPlane = XMPlaneNormalize(t.r[3] - t.r[2]);
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

	AABB AABB::GetByTransforming(const XMMATRIX & mat) const
	{
		// 变换所有顶点后，取极值做AABB
		XMFLOAT3 corners[8];
		for (int i = 0; i < 8; ++i)
		{
			XMVECTOR point = XMVector3Transform(corner(i), mat);
			XMStoreFloat3(&corners[i], point);
		}

		XMFLOAT3 min = corners[0];
		XMFLOAT3 max = corners[1];
		for (int i = 0; i < 8; ++i)
		{
			const XMFLOAT3& p = corners[i];

			if (p.x < min.x) min.x = p.x;
			if (p.y < min.y) min.y = p.y;
			if (p.z < min.z) min.z = p.z;

			if (p.x > max.x) max.x = p.x;
			if (p.y > max.y) max.y = p.y;
			if (p.z > max.z) max.z = p.z;
		}

		return AABB(XMLoadFloat3(&min), XMLoadFloat3(&max));
	}

	AABB AABB::GetByTransforming(const XMFLOAT4X4 & mat) const
	{
		return GetByTransforming(XMLoadFloat4x4(&mat));
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

	bool AABB::Intersects(const AABB& other) const
	{
		if ((mMax.x < other.mMin.x || mMin.x > other.mMax.x) ||
			(mMax.y < other.mMin.y || mMin.y > other.mMax.y) ||
			(mMax.z < other.mMin.z || mMin.z > other.mMax.z)) {
		
			return false;
		}

		return true;
	}

	bool AABB::Intersects(const Ray& other, F32* t) const
	{
		// 射线与轴对称包围盒相交检测
		// 当存在某一方向轴的最小值大于方向轴的最大值时，说明未相交
		if (Intersects(other.mOrigin)) {
			return true;
		}

		F32 maxValue = 0.0f;
		F32 minValue = 0.0f;

		// x axis
		F32 tx1 = (mMin.x - other.mOrigin.x) * other.mInvDirection.x;
		F32 tx2 = (mMax.x - other.mOrigin.x) * other.mInvDirection.x;
		minValue = std::min(tx1, tx2);
		maxValue = std::max(tx1, tx2);

		// y axis
		F32 ty1 = (mMin.y - other.mOrigin.y) * other.mInvDirection.y;
		F32 ty2 = (mMax.y - other.mOrigin.y) * other.mInvDirection.y;
		// 找到最大的最小变和最小的最大边
		minValue = std::max(minValue, std::min(ty1, ty2));
		maxValue = std::max(maxValue, std::max(ty1, ty2));

		// z axis
		F32 tz1 = (mMin.z - other.mOrigin.z) * other.mInvDirection.z;
		F32 tz2 = (mMax.z - other.mOrigin.z) * other.mInvDirection.z;
		// 找到最大的最小变和最小的最大边
		minValue = std::max(minValue, std::min(tz1, tz2));
		maxValue = std::max(maxValue, std::max(tz1, tz2));

		bool isIntersected = maxValue >= minValue;
		if (isIntersected && t != nullptr) {
			*t = minValue;
		}

		return isIntersected;
	}

	bool AABB::Intersects(const F32x3& pos) const
	{
		if (pos.x() < mMin.x || pos.x() > mMax.x) return false;
		if (pos.y() < mMin.y || pos.y() > mMax.y) return false;
		if (pos.z() < mMin.z || pos.z() > mMax.z) return false;
		return true;
	}

	void AABB::Serialize(Archive& archive, U32 seed)
	{
		archive >> mMin;
		archive >> mMax;
	}

	void AABB::Unserialize(Archive& archive) const
	{
		archive << mMin;
		archive << mMax;
	}

	bool Sphere::Intersects(const AABB& other) const
	{
		XMFLOAT3 closestPoint = XMFloat3Min(XMFloat3Max(other.mMin, mCenter), other.mMax);
		return XMDistance(mCenter, closestPoint) < mRadius;
	}

	bool Sphere::Intersects(const Sphere& other) const
	{
		return XMDistance(mCenter, other.mCenter) < (mRadius + other.mRadius);
	}

	bool Sphere::Intersects(const Ray& other) const
	{
		XMVECTOR origin = XMLoadFloat3(&other.mOrigin);
		XMVECTOR dir = XMLoadFloat3(&other.mDirection);
		XMVECTOR dist = DirectX::XMVector3LinePointDistance(origin, origin + dir, XMLoadFloat3(&mCenter));
		return DirectX::XMVectorGetX(dist) <= mRadius;
	}

	bool Ray::Intersects(const Sphere& sphere) const
	{
		return sphere.Intersects(*this);
	}

	bool Ray::Intersects(const AABB& aabb, F32* t) const
	{
		return aabb.Intersects(*this, t);
	}

}