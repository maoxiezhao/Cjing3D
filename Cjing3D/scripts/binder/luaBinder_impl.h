#pragma once

#include "scripts\binder\luaInvoke.h"
#include "scripts\binder\luaTools.h"
#include "scripts\binder\luaRef.h"
#include "scripts\binder\luaObject.h"

#include <functional>

namespace Cjing3D
{
	// check is lambda
	// from luaintf
	template<typename F>
	struct IsLambda
	{
		static constexpr bool value = std::is_class<F>::value;
	};

	template<typename R, typename... Args>
	struct IsLambda<std::function<R(Args...)>>
	{
		static constexpr bool value = false;
	};

	template<typename F>
	struct LambdaTraits : LambdaTraits<decltype(&F::operator())> {};	// lambda基本可以视为一个伪函数（类），且存在有operator()方法
																		// lambda::operator(),对operator()类型可得到一个类的成员函数指针
	template<typename F, typename R, typename... Args>
	struct LambdaTraits<R(F::*)(Args...)>
	{
		using FunctionType = std::function<R(Args...)>;
	};

	template<typename F, typename R, typename... Args>
	struct LambdaTraits<R(F::*)(Args...)const>
	{
		using FunctionType = std::function<R(Args...)>;
	};

	// Method function 
	template<typename T, typename F, typename R, typename... Args>
	struct BindClassMethodFuncCaller
	{
		// the called lua function
		static int Caller(lua_State* l)
		{
			return LuaTools::ExceptionBoundary(l, [&] {
				F& func = *static_cast<F*>(lua_touserdata(l, lua_upvalueindex(1)));
				T* obj = LuaObject::GetObject<T>(l, 1);
				LuaArgValueTuple<Args...> args;
				LuaInputArgs<Args...>::Get(l, 2, args);

				int resultCount = ClassMethodCaller<T, F, R, typename LuaArgs<Args>::LuaArgHolderType...>::Call(l, obj, func, args);
				return resultCount;
			});
		}
	};

	template<typename T, typename F, typename Args = F, typename Enable = void>
	struct BindClassMethodFunc;

	template<typename T, typename TF, typename R, typename... Args>
	struct BindClassMethodFunc<T, R(TF::*)(Args...)> :			// ==> <typename T, typename F, typename R, typename... Args>
		BindClassMethodFuncCaller<T, R(T::*)(Args...), R, Args...>
	{
		static_assert(std::is_base_of<TF, T>::value);
	};

	template<typename T, typename TF, typename R, typename... Args>
	struct BindClassMethodFunc<T, R(TF::*)(Args...)const> :
		BindClassMethodFuncCaller<T, R(T::*)(Args...)const, R, Args...>
	{
		static_assert(std::is_base_of<TF, T>::value);
	};

	// check enable
	template<typename T, typename F>
	struct BindClassMethodFunc<T, F, F, typename std::enable_if<std::is_function<F>::value>::type> :
		BindClassMethodFunc<T, F*, F*> {};

	// Method function ///////////////////////////////////////////////////////////////////////////////////////
	template<typename F, typename R, typename... Args>
	struct BindClassStaicFuncCaller
	{
		// the called lua function
		static int Caller(lua_State* l)
		{
			return LuaTools::ExceptionBoundary(l, [&] {
				F& func = *static_cast<F*>(lua_touserdata(l, lua_upvalueindex(1)));
				LuaArgValueTuple<Args...> args;
				LuaInputArgs<Args...>::Get(l, 1, args);

				int resultCount = StaticFunctionCaller<F, R, typename LuaArgs<Args>::LuaArgHolderType...>::Call(l, func, args);
				return resultCount;
			});
		}

		// convert labmda to function, so is can save by function pointer
		template<typename T>
		static F ConvertToFunction(const T& t)
		{
			return static_cast<F>(t);
		}
	};

	template<typename F, typename Args = F, typename Enable = void>
	struct BindClassStaticFunc;

	template<typename R, typename... Args>
	struct BindClassStaticFunc<std::function<R(Args...)>, std::function<R(Args...)>> :
		BindClassStaicFuncCaller<std::function<R(Args...)>, R, Args...> {};

	template<typename R, typename... Args>
	struct BindClassStaticFunc<R(*)(Args...), R(*)(Args...)> :
		BindClassStaicFuncCaller<R(*)(Args...), R, Args...> {};

	// check enable
	template<typename F>
	struct BindClassStaticFunc<F, F, typename std::enable_if<std::is_function<F>::value>::type> :
		BindClassStaticFunc<F*, F*> {};

	template<typename F>
	struct BindClassStaticFunc<F, F, typename std::enable_if<IsLambda<F>::value>::type> :
		BindClassStaticFunc<typename LambdaTraits<F>::FunctionType, typename LambdaTraits<F>::FunctionType> {};

	// Bind Module meta method
	struct BindModuleMetaMethod
	{
		static int Index(lua_State* l);
		static int NewIndex(lua_State* l);
	};

	// bind variable setter/getter, pass-by-value
	template<typename T>
	struct BindVariableMethod
	{
		static int Getter(lua_State*l)
		{
			return LuaTools::ExceptionBoundary(l, [&] {
				LuaTools::CheckAssertion(l, lua_islightuserdata(l, lua_upvalueindex(1)),
					"BindVariableMethod::Getter upvalue must is lightuserdata.");

				const T* p = static_cast<const T*>(lua_touserdata(l, lua_upvalueindex(1)));
				LuaTools::CheckAssertion(l, p != nullptr, "BindVariableMethod::Getter userdata is nullptr.");

				LuaTools::Push<T>(l, *p);
				return 1;
			});
		}

		static int Setter(lua_State*l)
		{
			return LuaTools::ExceptionBoundary(l, [&] {
				LuaTools::CheckAssertion(l, lua_islightuserdata(l, lua_upvalueindex(1)),
					"BindVariableMethod::Setter upvalue must is lightuserdata.");

				T* p = static_cast<T*>(lua_touserdata(l, lua_upvalueindex(1)));
				LuaTools::CheckAssertion(l, p != nullptr, "BindVariableMethod::Setter userdata is nullptr.");

				*p = LuaTools::Get<T>(l, 1);

				return 0;
			});
		}
	};

	// bind variable setter/getter
	// class member pointer: V T::*
	template<typename T, typename V>
	struct BindClassMemberMethod
	{
		static int Getter(lua_State*l)
		{
			return LuaTools::ExceptionBoundary(l, [&] {
				LuaTools::CheckAssertion(l, lua_isuserdata(l, lua_upvalueindex(1)),
					"BindClassMemberMethod::Getter upvalue must is lightuserdata.");

				// get class member pointer
				auto p = static_cast<V T::**>(lua_touserdata(l, lua_upvalueindex(1)));

				T* obj = LuaObject::GetObject<T>(l, 1);
				auto& value = (obj->**p);

				LuaTools::Push<V>(l, value);
				return 1;
			});
		}

		static int Setter(lua_State*l)
		{
			return LuaTools::ExceptionBoundary(l, [&] {
				LuaTools::CheckAssertion(l, lua_isuserdata(l, lua_upvalueindex(1)),
					"BindClassMemberMethod::Setter upvalue must is lightuserdata.");

				// get class member pointer
				auto p = static_cast<V T::**>(lua_touserdata(l, lua_upvalueindex(1)));

				T* obj = LuaObject::GetObject<T>(l, 1);
				auto value = LuaTools::Get<V>(l, 2);

				(obj->**p) = value;

				return 0;
			});
		}
	};
}