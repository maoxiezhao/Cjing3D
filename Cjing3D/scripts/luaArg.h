#pragma once

#include "scripts\luaTools.h"
#include "scripts\luaRef.h"

#include <tuple>

namespace Cjing3D {

	struct _lua_args {};

#define _LUA_ARGS_TYPE(...) _lua_args(*)(__VA_ARGS__) 
#define  _LUA_ARGS_(...) static_cast<_LUA_ARGS_TYPE(__VA_ARGS__)>(nullptr)

	template<typename T>
	struct LuaArgHolder
	{
		T& Get() { return mValue; }
		const T& Get()const { return mValue; }
		void Set(const T& value) { mValue = value; }
		T mValue;
	};
	template<typename T>
	struct LuaArgHolder<T&>
	{
		T& Get() { return *mValue; }
		void Set(T& value) { mValue = &value; }
		T* mValue;
	};

	template<typename T>
	struct LuaArgTypeTraits
	{
		using ArgType = typename std::result_of<decltype(&LuaType<T>::Get)(lua_State*, int)>::type;
		using ArgHolderType = LuaArgHolder<ArgType>;
	};

	// 从lua stack 中取出数据，并将数据赋给LuaArgHover
	template<typename ArgTypeTraits>
	struct LuaArgGetter
	{
		using ArgHolderType = typename ArgTypeTraits::ArgHolderType;
		using ArgType = typename ArgTypeTraits::ArgType;

		static int Get(lua_State*l, int index, ArgHolderType& holder)
		{
			holder.Set(LuaTools::Get<ArgType>(l, index));
			return 1;
		}
	};

	template<typename T>
	struct LuaArgSetter;

	template<typename T>
	struct LuaArgs
	{
		using LuaArgTraits = LuaArgTypeTraits<T>;
		using LuaArgHolderType = typename LuaArgTraits::ArgHolderType;

		static int Get(lua_State*l, int index, LuaArgHolderType& holder)
		{
			return LuaArgGetter<LuaArgTraits>::Get(l, index, holder);
		}
	};

	// 依次push当前参数
	template<typename... Args>
	struct LuaPushArgs;
	template<>
	struct LuaPushArgs<> { static void Push(lua_State*l) {} };

	template<typename P, typename... Args>
	struct LuaPushArgs<P, Args... >
	{
		static void Push(lua_State*l, P&& p, Args&&... args)
		{
			LuaTools::Push<P>(l, std::forward<P>(p));
			return LuaPushArgs<Args...>::Push(l, std::forward<Args>(args)...);
		}
	};

	template<typename... Args>
	struct LuaInputArgs;

	template<>
	struct LuaInputArgs<>
	{
		template<typename... T>
		static void Get(lua_State*l, int index, std::tuple<T...>& valueTuple) {}
	};

	template<typename P, typename... Args>
	struct LuaInputArgs<P, Args... >
	{
		template<typename... T>
		static void Get(lua_State*l, int index, std::tuple<T...>& valueTuple)
		{
			auto& value = std::get<sizeof...(T) - sizeof...(Args) - 1>(valueTuple);
			index += LuaArgs<P>::Get(l, index, value);
			return LuaInputArgs<Args...>::Get(l, index, valueTuple);
		}
	};

	template<typename... Args>
	using LuaArgValueTuple = std::tuple<typename LuaArgs<Args>::LuaArgHolderType...>;

}