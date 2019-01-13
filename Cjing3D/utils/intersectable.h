#pragma once

#include "utils\math.h"
#include "utils\geometry.h"

namespace Cjing3D
{
	// 轴对称包围盒
	class AABB final
	{
	public:
		XMVECTOR mMin;
		XMVECTOR mMax;

		AABB() : AABB(g_XMInfinity, -g_XMInfinity) {};
		AABB(FXMVECTOR p) : mMin(p), mMax(p) {};
		AABB(XMVECTOR min, XMVECTOR max) : mMin(min), mMax(max) {};
		AABB(const AABB& aabb) = default;
		AABB(AABB&& aabb) = default;
		~AABB() = default;

		AABB& operator= (const AABB& aabb) = default;
		AABB& operator= (AABB&& aabb) = default;

		const XMVECTOR XM_CALLCONV GetMin()const {
			return mMin;
		}
		const XMVECTOR XM_CALLCONV GetMax()const {
			return mMax;
		}
		const XMVECTOR XM_CALLCONV GetCenter()const {
			return 0.5f * (mMin + mMax);
		}
		const XMVECTOR XM_CALLCONV GetDiagonal()const {
			return (mMax - mMin);
		}
		const XMVECTOR XM_CALLCONV GetRadius()const {
			return 0.5f * GetDiagonal();
		}

		template< typename VertexT>
		static const AABB XM_CALLCONV Union(const AABB& aabb, const VertexT& vertex)
		{
			return Union(aabb, vertex.mPosition);
		}

		static const AABB XM_CALLCONV Union(const AABB& aabb, Point3 point)
		{
			return Union(aabb, XMLoad(point));
		}

		static const AABB XM_CALLCONV Union(const AABB& aabb, FXMVECTOR point)
		{
			return Union(aabb, AABB(point));
		}

		static const AABB XM_CALLCONV Union(const AABB& aabb1, const AABB& aabb2)
		{
			const auto pMin = XMVectorMin(aabb1.mMax, aabb2.mMin);
			const auto pMax = XMVectorMax(aabb1.mMax, aabb2.mMax);
			return AABB(pMin, pMax);
		}

		const XMVECTOR GetMaxPointAlongNormal(FXMVECTOR n)const
		{
			// 返回法线法向最大值
			// if n.x > 0 => control.x = 0xFFFFFFF otherwise control.x = 0x0
			const auto control = XMVectorGreaterOrEqual(n, DirectX::XMVectorZero());
			// ret.x = (control.x = 0xffffff)? mMax.x : mMin.x
			return XMVectorSelect(mMin, mMax, control);
		}

		AABB GetByTransforming(const XMFLOAT4X4& mat)const;

		void CopyFromOther(const AABB& aabb);
	};

	// 视锥体
	class Frustum
	{
	public:
		Frustum();
		~Frustum();

		void SetupFrustum(XMMATRIX transform);
		bool Overlaps(const AABB& aabb)const;

	private:
		union {
			struct {
				XMVECTOR mleftPlane;
				XMVECTOR mRightPlane;
				XMVECTOR mTopPlane;
				XMVECTOR mBottomPlane;
				XMVECTOR mFarPlane;
				XMVECTOR mNearPlane;
			};
			XMVECTOR mPlanes[6];
		};
	};
}