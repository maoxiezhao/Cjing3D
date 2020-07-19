#pragma once

#include "scripts\luaBinder.h"
#include "scripts\luaTypeMapping.h"
#include "utils\math.h"

namespace Cjing3D
{
	namespace LuaApi
	{
		class BindLuaVector;

		void BindLuaMatrixApi(lua_State* l);

		// bind lua vector, manual binding
		class BindLuaMatrix
		{
		public:
			XMMATRIX mValue;
			
			BindLuaMatrix(const XMMATRIX& value) :mValue(value) {}
			static int Create(lua_State* l);

			void Translate(const BindLuaVector& vec);
			void Rotation(const BindLuaVector& vec);
			void RotationQuaternion(const BindLuaVector& vec);
		};
	}
	template<>
	struct LuaTypeNormalMapping<XMMATRIX>
	{
		static void Push(lua_State*l, const XMMATRIX& value)
		{
			LuaApi::BindLuaMatrix matrix(value);
			LuaTools::Push<LuaApi::BindLuaMatrix>(l, matrix);
		}

		static XMMATRIX Get(lua_State*l, int index)
		{
			LuaApi::BindLuaMatrix matrix = LuaTools::Get<LuaApi::BindLuaMatrix>(l, index);
			return matrix.mValue;
		}

		static XMMATRIX Opt(lua_State* l, int index, const XMMATRIX& defValue)
		{
			return lua_isnoneornil(l, index) ? defValue : Get(l, index);
		}
	};
}