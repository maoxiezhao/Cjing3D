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

	public:
		AABB() : AABB(g_XMInfinity, -g_XMInfinity) {};
		AABB(FXMVECTOR p) : mMin(p), mMax(p) {};
		AABB(XMVECTOR min, XMVECTOR max) : mMin(min), mMax(max) {};
		AABB(const AABB& aabb) = default;
		AABB(AABB&& aabb) = default;
		~AABB() = default;

		AABB& operator= (const AABB& aabb) = default;
		AABB& operator= (AABB&& aabb) = default;

		static AABB CreateFromHalfWidth(FXMVECTOR position, FXMVECTOR range)
		{
			XMVECTOR minVec = position - range;
			XMVECTOR maxVec = position + range;
			return AABB(minVec, maxVec);
		}

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

		AABB GetByTransforming(const XMMATRIX& mat)const;
		AABB GetByTransforming(const XMFLOAT4X4& mat)const;
		XMMATRIX GetBoxMatrix()const;
		void CopyFromOther(const AABB& aabb);

		inline XMVECTOR corner(int index) const
		{
			F32x3(1.0f, 1.0f, 1.0f);
			auto _min = XMStore<F32x3>(mMin);
			auto _max = XMStore<F32x3>(mMax);
			switch (index)
			{
			case 0: return XMLoad(_min);
			case 1: return XMLoad(F32x3(_min[0], _max[1], _min[2]));
			case 2: return XMLoad(F32x3(_min[0], _max[1], _max[2]));
			case 3: return XMLoad(F32x3(_min[0], _min[1], _max[2]));
			case 4: return XMLoad(F32x3(_max[0], _min[1], _min[2]));
			case 5: return XMLoad(F32x3(_max[0], _max[1], _min[2]));
			case 6: return XMLoad(_max);
			case 7: return XMLoad(F32x3(_max[0], _min[1], _max[2]));
			}
			assert(0);
			return XMVECTOR();
		}

		inline void SetFromHalfWidth(XMFLOAT3 center, XMFLOAT3 range)
		{
			XMFLOAT3 _min = XMFLOAT3(center.x - range.x, center.y - range.y, center.z - range.z);
			XMFLOAT3 _max = XMFLOAT3(center.x + range.x, center.y + range.y, center.z + range.z);

			mMin = XMLoadFloat3(&_min);
			mMax = XMLoadFloat3(&_max);
		}
	};

	// 视锥体
	class Frustum
	{
	public:
		Frustum();
		~Frustum();

		void SetupFrustum(const XMFLOAT4X4& view, const XMFLOAT4X4& projection, float screenDepth);
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