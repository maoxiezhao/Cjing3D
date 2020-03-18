#pragma once

#include <DirectXMath.h>
#include <algorithm>

#include "array.h"

#undef min
#undef max

namespace Cjing3D {
	using namespace DirectX;

	using U8 = unsigned __int8;
	using U16 = unsigned __int16;
	using U32 = unsigned __int32;
	using U64 = unsigned __int64;

	using I8 = __int8;
	using I16 = __int16;
	using I32 = __int32;
	using I64 = __int64;

	using F32 = float;
	using F64 = double;

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

	inline const XMFLOAT4 XM_CALLCONV XMConvert(const F32x4& src){
		XMFLOAT4 result;
		XMStoreFloat4(&result, XMLoad(src));
		return result;
	}

	inline const XMFLOAT3 XM_CALLCONV XMConvert(const F32x3& src) {
		XMFLOAT3 result;
		XMStoreFloat3(&result, XMLoad(src));
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
}