#pragma once

#include "scripts\binder\luaCommon.h"
#include "scripts\binder\luaException.h"

#include <type_traits>
#include <condition_variable>

namespace Cjing3D
{
	// NOTE:
	// 对于对象的Push和Get
	// 默认类型和LuaRef已经通过LuaTypeNormalMapping<T>来设置，不同特化结构体提供了Push和Get接口
	// 而对于用户自定义的数据，需要通过LuaTypeClassMapping来映射，不过在LuaObject中已经提供了一个
	// 通用的映射实现

	// 对于额外需要映射的类型或者对象，可以通过重新特化实现LuaTypeNormalMapping或LuaTypeClassMapping实现

	//----------------------------------base define----------------------------------------

	template<typename T, typename ENABLED = void>
	struct LuaTypeNormalMapping;

	template<typename T, bool IsRef>
	struct LuaTypeClassMapping;

	struct LuaTypeExists
	{
		// 通过typename = decltype(T())判断是否存在该类型
		template<typename T, typename = decltype(T())>
		static std::true_type test(int);

		template<typename>
		static std::false_type test(...);
	};

	template<typename T>
	struct LuaTypeMappingExists
	{
		using type = decltype(LuaTypeExists::test<LuaTypeNormalMapping<T>>(0));
		static constexpr bool value = type::value;
	};

	template<typename T>
	struct LuaType : 
		std::conditional<
		std::is_class<typename std::decay<T>::type>::value && 
			!(LuaTypeMappingExists<typename std::decay<T>::type>::value),
		LuaTypeClassMapping<typename std::decay<T>::type, std::is_reference<T>::value>,
		LuaTypeNormalMapping<typename std::decay<T>::type>>::type
	{};

	inline int GetPositiveIndex(lua_State * l, int index)
	{
		int positiveIndex = index;
		if (index < 0 && index >= -lua_gettop(l))
			positiveIndex = lua_gettop(l) + index + 1;

		return positiveIndex;
	}

	//----------------------------------normal type mapping----------------------------------------

	template<typename T>
	struct LuaTypeNumberMapping
	{
		static void Push(lua_State*l, const T&value)
		{
			lua_pushnumber(l, static_cast<lua_Number>(value));
		}

		static T Get(lua_State*l, int index)
		{
			index = GetPositiveIndex(l, index);
			if (!lua_isnumber(l, index))
				LuaException::ArgError(l, index, std::string("Excepted:Number, got ") + luaL_typename(l, index));

			return static_cast<T>(lua_tonumber(l, index));
		}

		static T Opt(lua_State* l, int index, T defValue)
		{
			return static_cast<T>(luaL_optnumber(l, index, defValue));
		}
	};

	template<> struct LuaTypeNormalMapping<float> : LuaTypeNumberMapping<float> {};
	template<> struct LuaTypeNormalMapping<double> : LuaTypeNumberMapping<double> {};

	template<typename T>
	struct LuaTypeIntegerMapping
	{
		static void Push(lua_State*l, const T&value)
		{
			lua_pushinteger(l, static_cast<lua_Integer>(value));
		}

		static T Get(lua_State*l, int index)
		{
			index = GetPositiveIndex(l, index);
			if (!lua_isinteger(l, index))
				LuaException::ArgError(l, index, std::string("Excepted:Integer, got ") + luaL_typename(l, index));

			return static_cast<T>(lua_tointeger(l, index));
		}

		static T Opt(lua_State* l, int index, T defValue)
		{
			return static_cast<T>(luaL_optinteger(l, index, defValue));
		}
	};

	template<> struct LuaTypeNormalMapping<short> : LuaTypeIntegerMapping<short> {};
	template<> struct LuaTypeNormalMapping<int> : LuaTypeIntegerMapping<int> {};
	template<> struct LuaTypeNormalMapping<long> : LuaTypeIntegerMapping<long> {};
	template<> struct LuaTypeNormalMapping<unsigned short> : LuaTypeIntegerMapping<short> {};
	template<> struct LuaTypeNormalMapping<unsigned int> : LuaTypeIntegerMapping<int> {};
	template<> struct LuaTypeNormalMapping<unsigned long> : LuaTypeIntegerMapping<long> {};

	template<>
	struct LuaTypeNormalMapping<bool>
	{
		static void Push(lua_State*l, bool value)
		{
			lua_pushboolean(l, value);
		}

		static bool Get(lua_State*l, int index)
		{
			index = GetPositiveIndex(l, index);
			if (!lua_isboolean(l, index))
				LuaException::ArgError(l, index, std::string("Excepted:boolean, got ") + luaL_typename(l, index));

			return lua_toboolean(l, index);
		}

		static bool Opt(lua_State* l, int index, bool defValue)
		{
			return lua_isnone(l, index) ? defValue : lua_toboolean(l, index);
		}
	};

#if defined(LUA_COMPAT_APIINTCASTS)

	template<typename T>
	struct LuaTypeUnsignedMapping
	{
		static void Push(lua_State*l, const T&value)
		{
			lua_pushunsigned(l, static_cast<lua_Unsigned>(value));
		}

		static T Get(lua_State*l, int index)
		{
			index = GetPositiveIndex(l, index);
			return static_cast<T>(lua_tounsigned(l, index));
		}
	};

	template<> struct LuaTypeNormalMapping<unsigned char> : LuaTypeNumberMapping<unsigned char> {};
	template<> struct LuaTypeNormalMapping<unsigned int> : LuaTypeNumberMapping<unsigned int> {};
	template<> struct LuaTypeNormalMapping<unsigned long> : LuaTypeNumberMapping<unsigned long> {};

#endif

	template<>
	struct LuaTypeNormalMapping<std::string>
	{
		static void Push(lua_State*l, const std::string& value)
		{
			lua_pushstring(l, value.c_str());
		}

		static std::string Get(lua_State*l, int index)
		{
			index = GetPositiveIndex(l, index);
			if (!lua_isstring(l, index))
				LuaException::ArgError(l, index, std::string("Excepted:String, got ") + luaL_typename(l, index));

			return std::string(lua_tostring(l, index));
		}

		static std::string Opt(lua_State* l, int index, const std::string& defValue)
		{
			return lua_isnoneornil(l, index) ? defValue : Get(l, index);
		}
	};

	template<>
	struct LuaTypeNormalMapping<char*>
	{
		static void Push(lua_State*l, const char* value)
		{
			lua_pushstring(l, value);
		}

		static const char* Get(lua_State* l, int index)
		{
			index = GetPositiveIndex(l, index);
			if (!lua_isstring(l, index))
				LuaException::ArgError(l, index, std::string("Excepted:char*, got ") + luaL_typename(l, index));

			return lua_tostring(l, index);
		}

		static const char* Opt(lua_State* l, int index, const char* defValue)
		{
			return lua_isnoneornil(l, index) ? defValue : Get(l, index);
		}
	};

	template<>
	struct LuaTypeNormalMapping<lua_CFunction>
	{
		static void Push(lua_State*l, lua_CFunction value)
		{
			lua_pushcfunction(l, value);
		}

		static lua_CFunction Get(lua_State*l, int index)
		{
			return lua_tocfunction(l, index);
		}
	};

	// enum类型以Integer类型来处理
	template<typename T>
	struct LuaTypeNormalMapping<T, typename std::enable_if<std::is_enum<T>::value, void>::type> :
		LuaTypeIntegerMapping<T> {};

}