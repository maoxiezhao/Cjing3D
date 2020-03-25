#pragma once

#include "math.h"

namespace Cjing3D {
	/**
	*	\brief 纹理坐标类
	*/
	class UV : public F32x2
	{
	public:
		constexpr UV() = default;
		constexpr UV(F32 u, F32 v) : F32x2(u, v) {}
		~UV() = default;

		constexpr F32 GetU()const { return operator[](0); }
		constexpr F32 GetV()const { return operator[](1); }

		void SetU(F32 v) { operator[](0) = v; }
		void SetV(F32 v) { operator[](1) = v; }
	};

	/**
	*	\brief 三维点类
	*/
	class Point3 : public F32x3
	{
	public:
		constexpr Point3() = default;
		constexpr Point3(F32 x, F32 y, F32 z) : F32x3(x, y, z) {}
		~Point3() = default;

		constexpr F32 GetX()const { return operator[](0); }
		constexpr F32 GetY()const { return operator[](1); }
		constexpr F32 GetZ()const { return operator[](2); }

		void SetX(F32 v) { operator[](0) = v; }
		void SetY(F32 v) { operator[](1) = v; }
		void SetZ(F32 v) { operator[](2) = v; }
	};

	/**
	*	\brief 三维空间向量
	*/
	class Vertex3 : public F32x3 {
	public:
		constexpr Vertex3() = default;
		constexpr Vertex3(F32 x, F32 y, F32 z) : F32x3(x, y, z) {}
		constexpr explicit Vertex3(F32x3 v) :F32x3(std::move(v)) {}
		~Vertex3() = default;

		constexpr F32 GetX()const { return operator[](0); }
		constexpr F32 GetY()const { return operator[](1); }
		constexpr F32 GetZ()const { return operator[](2); }

		void SetX(F32 v) { operator[](0) = v; }
		void SetY(F32 v) { operator[](1) = v; }
		void SetZ(F32 v) { operator[](2) = v; }
	};

	/**
	*	\brief 三维空间法向量
	*/
	class Normal3 : public Vertex3 {
	public:
		constexpr Normal3() = default;
		constexpr Normal3(F32 x, F32 y, F32 z) : Vertex3(x, y, z) {}
		constexpr explicit Normal3(F32x3 v) : Vertex3(std::move(v)) {}
	};

	class Rect
	{
	public:
		F32 mLeft = 0.0f;
		F32 mTop = 0.0f;
		F32 mRight = 0.0f;
		F32 mBottom = 0.0f;

		Rect() = default;
		Rect(F32 left, F32 top, F32 right, F32 bottom) : mLeft(left), mTop(top), mRight(right), mBottom(bottom) {}
		Rect(F32x2 pos, F32x2 size) :mLeft(pos[0]), mTop(pos[1]), mRight(pos[0] + size[0]), mBottom(pos[1] + size[1]) {}

		void SetSize(F32x2 size) { mRight = mLeft + size[0]; mBottom = mTop + size[1]; }
		void SetPos(F32x2 pos) { F32x2 offset = pos - GetPos(); Offset(offset); }

		F32x2 GetPos()const { return { mLeft, mTop }; }
		F32x2 GetSize()const { return { mRight - mLeft, mBottom - mTop }; }
		F32 GetHeight()const { return mBottom - mTop; }
		F32 GetWidth()const { return mRight - mLeft; }
		F32x2 GetCenter()const { return {(mRight + mLeft) * 0.5f, (mTop + mBottom) * 0.5f }; }

		bool Intersects(const Rect& rect)const;
		bool Intersects(const F32x2& point)const;

	    inline void Union(const Rect& rect)
		{
			mLeft = std::min(mLeft, rect.mLeft);
			mTop = std::min(mTop, rect.mTop);
			mRight = std::max(mRight, rect.mRight);
			mBottom = std::max(mBottom, rect.mBottom);
		}

		Rect& Offset(const F32x2& offset);
	};
}