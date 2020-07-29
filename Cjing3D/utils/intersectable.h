#pragma once

#include "utils\math.h"
#include "utils\geometry.h"

#include "helper\binaryArchive.h"

namespace Cjing3D
{
	class AABB;
	class Ray;

	class Sphere final
	{
	public:
		XMFLOAT3 mCenter;
		F32 mRadius;

		Sphere() :mCenter(XMFLOAT3(0.0f, 0.0f, 0.0f)), mRadius(0.0f) {}
		Sphere(XMFLOAT3 center, F32 radius) : mCenter(center), mRadius(radius) {}

		bool Intersects(const AABB& other)const;
		bool Intersects(const Sphere& other)const;
		bool Intersects(const Ray& other)const;
	};

	// 轴对称包围盒
	class AABB final
	{
	public:
		XMFLOAT3 mMin;
		XMFLOAT3 mMax;

	public:
		AABB() : AABB(g_XMInfinity, -g_XMInfinity) {};
		AABB(XMVECTOR p) { XMStoreFloat3(&mMin, p); XMStoreFloat3(&mMax, p); };
		AABB(XMVECTOR min, XMVECTOR max) { XMStoreFloat3(&mMin, min); XMStoreFloat3(&mMax, max); };
		AABB(XMFLOAT3 p) : mMin(p), mMax(p) {};
		AABB(XMFLOAT3 min, XMFLOAT3 max) : mMin(min), mMax(max) {};
		AABB(F32x3 min, F32x3 max) : mMin(XMConvert(min)), mMax(XMConvert(max)) {}
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
			return XMLoadFloat3(&mMin);
		}
		const XMVECTOR XM_CALLCONV GetMax()const {
			return XMLoadFloat3(&mMax);
		}
		const XMVECTOR XM_CALLCONV GetCenter()const {

			return (GetMin() + GetMax()) * 0.5f;
		}
		const XMVECTOR XM_CALLCONV GetDiagonal()const {
			return (GetMax() - GetMin());
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
			const auto pMin = XMVectorMin(aabb1.GetMin(), aabb2.GetMin());
			const auto pMax = XMVectorMax(aabb1.GetMax(), aabb2.GetMax());
			return AABB(pMin, pMax);
		}

		const XMVECTOR GetMaxPointAlongNormal(FXMVECTOR n)const
		{
			// 返回法线法向最大值
			// if n.x > 0 => control.x = 0xFFFFFFF otherwise control.x = 0x0
			const auto control = XMVectorGreaterOrEqual(n, DirectX::XMVectorZero());
			// ret.x = (control.x = 0xffffff)? mMax.x : mMin.x
			return XMVectorSelect(GetMin(), GetMax(), control);
		}

		AABB GetByTransforming(const XMMATRIX& mat)const;
		AABB GetByTransforming(const XMFLOAT4X4& mat)const;
		XMMATRIX GetBoxMatrix()const;
		void CopyFromOther(const AABB& aabb);

		bool Intersects(const AABB& other)const;
		bool Intersects(const Ray& other, F32* t = nullptr)const;
		bool Intersects(const F32x3& pos)const;

		inline XMVECTOR corner(int index) const
		{
			F32x3(1.0f, 1.0f, 1.0f);
			auto _min = XMStore<F32x3>(GetMin());
			auto _max = XMStore<F32x3>(GetMax());
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
			mMin = XMFLOAT3(center.x - range.x, center.y - range.y, center.z - range.z);
			mMax = XMFLOAT3(center.x + range.x, center.y + range.y, center.z + range.z);
		}

		virtual void Serialize(Archive& archive, U32 seed = 0);
		virtual void Unserialize(Archive& archive)const;
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

	// Direct::BoundingFrustum
	inline DirectX::BoundingFrustum CreateBoundingFrustum(const XMMATRIX& projection)
	{
		BoundingFrustum newFrustum;
		BoundingFrustum::CreateFromMatrix(newFrustum, projection);
		std::swap(newFrustum.Near, newFrustum.Far);
		return newFrustum;
	}

	inline void TransformBoundingFrustum(DirectX::BoundingFrustum& frustum, const XMMATRIX& transform)
	{
		frustum.Transform(frustum, transform);
		XMStoreFloat4(&frustum.Orientation, XMQuaternionNormalize(XMLoadFloat4(&frustum.Orientation)));
	}

	class Ray
	{
	public:
		XMFLOAT3 mOrigin = XMFLOAT3(0.0f, 0.0f, 0.0f);
		XMFLOAT3 mDirection = XMFLOAT3(0.0f, 0.0f, 1.0f);
		XMFLOAT3 mInvDirection = XMFLOAT3(0.0f, 0.0f, 1.0f);

		Ray() = default;
		Ray(const XMFLOAT3& origin, const XMFLOAT3& direction) :
			mOrigin(origin), 
			mDirection(direction) 
		{
			XMStoreFloat3(&mInvDirection, XMVectorReplicate(1.0f) / XMLoadFloat3(&mDirection));
		}
		Ray(const XMVECTOR& origin, const XMVECTOR& direction) 
		{
			XMStoreFloat3(&mOrigin, origin);
			XMStoreFloat3(&mDirection, direction);
			XMStoreFloat3(&mInvDirection, XMVectorReplicate(1.0f) / XMLoadFloat3(&mDirection));
		}

		bool Intersects(const Sphere& sphere)const;
		bool Intersects(const AABB& aabb, F32* t = nullptr)const;
	};
}