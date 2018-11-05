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
}