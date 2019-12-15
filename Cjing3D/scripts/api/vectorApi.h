#pragma once

#include "scripts\luaBinder.h"
#include "scripts\luaTypeMapping.h"
#include "utils\math.h"

namespace Cjing3D
{
namespace LuaApi
{
	class BindLuaMatrix;

	void BindLuaVectorApi(lua_State* l);

	// bind lua vector, manual binding
	class BindLuaVector
	{
	public:
		BindLuaVector(const XMVECTOR& value) :mValue(value) {}
		~BindLuaVector();
		static int Create(lua_State* l);

		F32 GetX() { return XMVectorGetX(mValue); };
		F32 GetY() { return XMVectorGetY(mValue); };
		F32 GetZ() { return XMVectorGetZ(mValue); };
		F32 GetW() { return XMVectorGetW(mValue); };

		void SetX(F32 x) { mValue = XMVectorSetX(mValue, x); }
		void SetY(F32 x) { mValue = XMVectorSetY(mValue, x); }
		void SetZ(F32 x) { mValue = XMVectorSetZ(mValue, x); }
		void SetW(F32 x) { mValue = XMVectorSetW(mValue, x); }
		void SetXYZ(F32 x, F32 y, F32 z) {
			mValue = XMVectorSetX(mValue, x);
			mValue = XMVectorSetY(mValue, y);
			mValue = XMVectorSetZ(mValue, z);
		}

		void Normalize() { mValue = XMVector3Normalize(mValue); }
		F32 Length() { return (F32)XMVectorGetX(XMVector3Length(mValue)); }

		static BindLuaVector Multiply(const BindLuaVector& v1, const BindLuaVector& v2);
		static BindLuaVector MultiplyWithNumber(const BindLuaVector& v, const F32& number);

		static BindLuaVector Transform(const BindLuaVector& vec, const BindLuaMatrix& mat);
		static BindLuaVector TransformNormal(const BindLuaVector& vec, const BindLuaMatrix& mat);

		XMVECTOR mValue;
	};
}
	template<>
	struct LuaTypeNormalMapping<I32x2>
	{
		static void Push(lua_State*l, const I32x2& value)
		{
			LuaApi::BindLuaVector vector(XMLoad(value));
			LuaTools::Push<LuaApi::BindLuaVector>(l, vector);
		}

		static I32x2 Get(lua_State*l, int index)
		{
			I32x2 result;
			LuaApi::BindLuaVector vector = LuaTools::Get<LuaApi::BindLuaVector>(l, index);
			result[0] = (I32)XMVectorGetX(vector.mValue);
			result[1] = (I32)XMVectorGetY(vector.mValue);

			return result;
		}
	};

	template<>
	struct LuaTypeNormalMapping<DirectX::XMFLOAT3>
	{
		static void Push(lua_State*l, const XMFLOAT3& value)
		{
			LuaApi::BindLuaVector vector(XMLoadFloat3(&value));
			LuaTools::Push<LuaApi::BindLuaVector>(l, vector);
		}

		static XMFLOAT3 Get(lua_State*l, int index)
		{
			XMFLOAT3 result;
			LuaApi::BindLuaVector vector = LuaTools::Get<LuaApi::BindLuaVector>(l, index);
			XMStoreFloat3(&result, vector.mValue);

			return result;
		}
	};

	template<>
	struct LuaTypeNormalMapping<F32x3>
	{
		static void Push(lua_State*l, const F32x3& value)
		{
			LuaApi::BindLuaVector vector(XMLoad(value));
			LuaTools::Push<LuaApi::BindLuaVector>(l, vector);
		}

		static F32x3 Get(lua_State*l, int index)
		{
			LuaApi::BindLuaVector vector = LuaTools::Get<LuaApi::BindLuaVector>(l, index);
			F32x3 result = XMStore<F32x3>(vector.mValue);
			return result;
		}
	};

	template<>
	struct LuaTypeNormalMapping<DirectX::XMFLOAT4>
	{
		static void Push(lua_State*l, const XMFLOAT4& value)
		{
			LuaApi::BindLuaVector vector(XMLoadFloat4(&value));
			LuaTools::Push<LuaApi::BindLuaVector>(l, vector);
		}

		static XMFLOAT4 Get(lua_State*l, int index)
		{
			XMFLOAT4 result;
			LuaApi::BindLuaVector vector = LuaTools::Get<LuaApi::BindLuaVector>(l, index);
			XMStoreFloat4(&result, vector.mValue);
			return result;
		}
	};

	template<>
	struct LuaTypeNormalMapping<F32x4>
	{
		static void Push(lua_State*l, const F32x4& value)
		{
			LuaApi::BindLuaVector vector(XMLoad(value));
			LuaTools::Push<LuaApi::BindLuaVector>(l, vector);
		}

		static F32x4 Get(lua_State*l, int index)
		{
			LuaApi::BindLuaVector vector = LuaTools::Get<LuaApi::BindLuaVector>(l, index);
			F32x4 result = XMStore<F32x4>(vector.mValue);
			return result;
		}
	};
}