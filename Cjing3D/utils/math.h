#pragma once

#include <DirectXMath.h>
#include <algorithm>

#include "common\definitions.h"
#include "array.h"

#undef min
#undef max

namespace Cjing3D {
	using namespace DirectX;

	using U32x2 = Array<U32, 2>;
	using U32x3 = Array<U32, 3>;
	using U32x4 = Array<U32, 4>;

	using F32x2 = Array<F32, 2>;
	using F32x3 = Array<F32, 3>;
	using F32x4 = Array<F32, 4>;

	using I32x2 = Array<I32, 2>;
	using I32x3 = Array<I32, 3>;
	using I32x4 = Array<I32, 4>;

	/*********************************************************
	*	\brief VertorType To XMVector
	*********************************************************/
	inline const XMVECTOR XM_CALLCONV XMLoad(const I32x2& src) {
		XMVECTOR result = XMVectorZero();
		result = XMVectorSetX(result, (float)src[0]);
		result = XMVectorSetY(result, (float)src[1]);
		return result;
	}

	inline const XMVECTOR XM_CALLCONV XMLoad(const U32x2& src) {
		return XMLoadUInt2(reinterpret_cast<const XMUINT2*>(&src));
	}

	inline const XMVECTOR XM_CALLCONV XMLoad(const U32x3& src) {
		return XMLoadUInt3(reinterpret_cast<const XMUINT3*>(&src));
	}

	inline const XMVECTOR XM_CALLCONV XMLoad(const U32x4& src) {
		return XMLoadUInt4(reinterpret_cast<const XMUINT4*>(&src));
	}

	inline const XMVECTOR XM_CALLCONV XMLoad(const F32x2& src) {
		XMVECTOR result = XMVectorZero();
		result = XMVectorSetX(result, src[0]);
		result = XMVectorSetY(result, src[1]);
		return result;
	}

	inline const XMVECTOR XM_CALLCONV XMLoad(const F32x3& src) {
		return XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&src));
	}

	inline const XMVECTOR XM_CALLCONV XMLoad(const F32x4& src) {
		return XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&src));
	}

	inline const XMFLOAT2 XM_CALLCONV XMConvert(const F32x2& src) {
		XMFLOAT2 result;
		XMStoreFloat2(&result, XMLoad(src));
		return result;
	}

	inline const XMFLOAT3 XM_CALLCONV XMConvert(const F32x3& src) {
		XMFLOAT3 result;
		XMStoreFloat3(&result, XMLoad(src));
		return result;
	}

	inline const XMFLOAT4 XM_CALLCONV XMConvert(const F32x4& src) {
		XMFLOAT4 result;
		XMStoreFloat4(&result, XMLoad(src));
		return result;
	}

	inline const XMUINT2 XM_CALLCONV XMConvert(const U32x2& src) {
		XMUINT2 result;
		XMStoreUInt2(&result, XMLoad(src));
		return result;
	}

	inline const XMUINT3 XM_CALLCONV XMConvert(const U32x3& src) {
		XMUINT3 result;
		XMStoreUInt3(&result, XMLoad(src));
		return result;
	}

	inline const XMUINT4 XM_CALLCONV XMConvert(const U32x4& src) {
		XMUINT4 result;
		XMStoreUInt4(&result, XMLoad(src));
		return result;
	}

	/*********************************************************
	*	\brief XMVector To VertorType
	*********************************************************/
	template<typename T>
	const T XM_CALLCONV XMStore(FXMVECTOR src);

	template<>
	inline const F32 XM_CALLCONV XMStore(FXMVECTOR src) {
		F32 dst;
		XMStoreFloat(&dst, src);
		return dst;
	}

	template<>
	inline const F32x2 XM_CALLCONV XMStore(FXMVECTOR src) {
		F32x2 dst;
		XMStoreFloat2(reinterpret_cast<XMFLOAT2*>(&dst), src);
		return dst;
	}

	template<>
	inline const F32x3 XM_CALLCONV XMStore(FXMVECTOR src) {
		F32x3 dst;
		XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&dst), src);
		return dst;
	}

	template<>
	inline const F32x4 XM_CALLCONV XMStore(FXMVECTOR src) {
		F32x4 dst;
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&dst), src);
		return dst;
	}

	// SDBM Hash
	inline unsigned int SDBMHash(unsigned int hash, unsigned char c)
	{
		return c + (hash << 6) + (hash << 16) - hash;
	}

	inline bool IsF32EqualZero(F32 v)
	{
		return abs(v) < 0.000001f;
	}

	inline bool IsF32EqualF32(F32 v1, F32 v2)
	{
		return abs(v2 - v1) < 0.000001f;
	}

	inline F32x2 F32x2Max(F32x2 a, F32x2 b)
	{
		return F32x2(
			std::max(a[0], b[0]),
			std::max(a[1], b[1])
		);
	}

	inline F32x2 F32x2Min(F32x2 a, F32x2 b)
	{
		return F32x2(
			std::min(a[0], b[0]),
			std::min(a[1], b[1])
		);
	}

	inline F32x3 F32x3Max(F32x3 a, F32x3 b)
	{
		return F32x3(
			std::max(a[0], b[0]),
			std::max(a[1], b[1]),
			std::max(a[2], b[2])
		);
	}

	inline F32x3 F32x3Min(F32x3 a, F32x3 b)
	{
		return F32x3(
			std::min(a[0], b[0]),
			std::min(a[1], b[1]),
			std::min(a[2], b[2])
		);
	}

	inline XMFLOAT3 XMFloat3Max(XMFLOAT3 a, XMFLOAT3 b)
	{
		return XMFLOAT3(
			std::max(a.x, b.x),
			std::max(a.y, b.y),
			std::max(a.z, b.z)
		);
	}

	inline XMFLOAT3 XMFloat3Min(XMFLOAT3 a, XMFLOAT3 b)
	{
		return XMFLOAT3(
			std::min(a.x, b.x),
			std::min(a.y, b.y),
			std::min(a.z, b.z)
		);
	}

	inline F32x3 F32x3Normalize(F32x3 n)
	{
		return XMStore<F32x3>(XMVector3Normalize(XMLoad(n)));
	}

	template<typename T>
	inline void HashCombine(U32& seed, const T& value)
	{
		std::hash<T> hasher;
		seed ^= hasher(value) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	}

	inline constexpr bool Collision2D(const F32x2& box1Pos, const F32x2& box1Size, const F32x2& box2Pos, const F32x2& box2Size)
	{
		if (box1Pos[0] + box1Size[0] < box2Pos[0])
			return false;
		else if (box1Pos[0] > box2Pos[0] + box2Size[0])
			return false;
		else if (box1Pos[1] + box1Size[1] < box2Pos[1])
			return false;
		else if (box1Pos[1] > box2Pos[1] + box2Size[1])
			return false;

		return true;
	}

	inline XMFLOAT3 QuaternionToRollPitchYaw(const XMFLOAT4& quaternion)
	{
		F32 roll  = atan2f(2 * quaternion.x * quaternion.w - 2 * quaternion.y * quaternion.z, 1 - 2 * quaternion.x * quaternion.x - 2 * quaternion.z * quaternion.z);
		F32 pitch = atan2f(2 * quaternion.y * quaternion.w - 2 * quaternion.x * quaternion.z, 1 - 2 * quaternion.y * quaternion.y - 2 * quaternion.z * quaternion.z);
		F32 yaw   = asinf(2 * quaternion.x * quaternion.y + 2 * quaternion.z * quaternion.w);

		return XMFLOAT3(roll, pitch, yaw);
	}

	inline F32 DistanceEstimated(F32x3 v1, F32x3 v2)
	{
		XMVECTOR sub = XMLoad(v1 - v2);
		return XMStore<F32>(XMVector3LengthEst(sub));
	}

	// 使用DirectXMath SIMD加速运算
	inline F32 XMDistance(const XMVECTOR& v1, const XMVECTOR& v2)
	{
		XMVECTOR sub = XMVectorSubtract(v1, v2);
		XMVECTOR length = XMVector3Length(sub);

		float ret = 0.0f;
		XMStoreFloat(&ret, length);
		return ret;
	}

	inline F32 XMDistance(XMFLOAT3 v1, XMFLOAT3 v2)
	{
		return XMDistance(XMLoadFloat3(&v1), XMLoadFloat3(&v2));
	}

	inline bool XMMatrixCompare(const XMMATRIX& m1, const XMMATRIX& m2)
	{
		return memcmp(&m1, &m2, sizeof(XMFLOAT4X4)) == 0;
	}
}