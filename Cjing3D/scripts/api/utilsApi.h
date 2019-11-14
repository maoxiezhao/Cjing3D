#pragma once

#include "scripts\luaBinder.h"
#include "scripts\luaTypeMapping.h"
#include "utils\math.h"

namespace Cjing3D
{
namespace LuaApi
{
	// bind lua vector, manual binding
	class BindLuaVector
	{
	public:
		BindLuaVector(const XMVECTOR& value) :mValue(value) {}
		static int Create(lua_State* l);

		F32 GetX() { return XMVectorGetX(mValue); };
		F32 GetY() { return XMVectorGetY(mValue); };
		F32 GetZ() { return XMVectorGetZ(mValue); };
		F32 GetW() { return XMVectorGetW(mValue); };

		void SetX(F32 x) { mValue = XMVectorSetX(mValue, x); }
		void SetY(F32 x) { mValue = XMVectorSetY(mValue, x); }
		void SetZ(F32 x) { mValue = XMVectorSetZ(mValue, x); }
		void SetW(F32 x) { mValue = XMVectorSetW(mValue, x); }

		XMVECTOR mValue;
	};
}
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
}