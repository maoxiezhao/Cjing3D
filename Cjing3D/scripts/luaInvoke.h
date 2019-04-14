#pragma once

#include "scripts\luaTools.h"

namespace Cjing3D
{
	// use size_t... INDEX to get args
	template<typename T, typename TUPLE, size_t N, size_t... INDEX>
	struct ClassStructorDispatchCaller : ClassStructorDispatchCaller<T, TUPLE, N - 1, N - 1, INDEX...> {};

	template<typename T, typename TUPLE, size_t... INDEX>
	struct ClassStructorDispatchCaller<T, TUPLE, 0, INDEX...>
	{
		static void Call(void* mem, TUPLE& args)
		{
			new(mem)T(std::get<INDEX>(args).Get()...);
		}
	};
	// Class Constructor
	template<typename T, typename... Args>
	struct ClassStructorCaller
	{
		static void Call(void* mem, std::tuple<Args...>& args)
		{
			ClassStructorDispatchCaller<T, std::tuple<Args...>, sizeof...(Args)>::Call(mem, args);
		}
	};

	// use size_t... INDEX to get args
	template<typename T, typename FUNC, typename R, typename TUPLE, size_t N, size_t... INDEX>
	struct ClassMethodDispatchCaller : ClassMethodDispatchCaller<T, FUNC, R, TUPLE, N - 1, N - 1, INDEX...> {};

	template<typename T, typename FUNC, typename R, typename TUPLE, size_t... INDEX>
	struct ClassMethodDispatchCaller<T, FUNC, R, TUPLE, 0, INDEX...>
	{
		static R Call(T* t, const FUNC& func, TUPLE& args)
		{
			return (t->*func)(std::get<INDEX>(args).Get()...);
		}
	};
	// Class Method Caller
	template<typename T, typename FUNC, typename R, typename... Args>
	struct ClassMethodCaller
	{
		static int Call(lua_State*l, T* t, const FUNC& func, std::tuple<Args...>& args)
		{
			R result = ClassMethodDispatchCaller<T, FUNC, R, std::tuple<Args...>, sizeof...(Args)>::Call(t, func, args);
			LuaTools::Push<R>(l, result);
			return 1;
		}
	};
	template<typename T, typename FUNC, typename... Args>
	struct ClassMethodCaller<T, FUNC, void, Args...>
	{
		static int Call(lua_State*l, T* t, const FUNC& func, std::tuple<Args...>& args)
		{
			ClassMethodDispatchCaller<T, FUNC, void, std::tuple<Args...>, sizeof...(Args)>::Call(t, func, args);
			return 0;
		}
	};

	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Class Method Caller
	template<typename FUNC, typename R, typename TUPLE, size_t N, size_t... INDEX>
	struct StaticFunctionDispatchCaller : StaticFunctionDispatchCaller<FUNC, R, TUPLE, N - 1, N - 1, INDEX...> {};

	template<typename FUNC, typename R, typename TUPLE, size_t... INDEX>
	struct StaticFunctionDispatchCaller<FUNC, R, TUPLE, 0, INDEX...>
	{
		static R Call(const FUNC& func, TUPLE& args)
		{
			return func(std::get<INDEX>(args).Get()...);
		}
	};
	
	template<typename FUNC, typename R, typename... Args>
	struct StaticFunctionCaller
	{
		static int Call(lua_State*l, const FUNC& func, std::tuple<Args...>& args)
		{
			R result = StaticFunctionDispatchCaller<FUNC, R, std::tuple<Args...>, sizeof...(Args)>::Call(func, args);
			LuaTools::Push<R>(l, result);
			return 1;
		}
	};
	template<typename FUNC, typename... Args>
	struct StaticFunctionCaller<FUNC, void, Args...>
	{
		static int Call(lua_State*l, const FUNC& func, std::tuple<Args...>& args)
		{
			StaticFunctionDispatchCaller<FUNC, void, std::tuple<Args...>, sizeof...(Args)>::Call(func, args);
			return 0;
		}
	};
	
}