#pragma once

#include "scripts\luaTypeMapping.h"

#include <string>
#include <type_traits>

namespace Cjing3D
{
class LuaRef;

namespace LuaTools
{
	void CheckType(lua_State*l, int index, int exceptedType);
	int  GetPositiveIndex(lua_State*l, int index);
	void PrintLuaStack(lua_State*l);
	void PrintInIndex(lua_State*l, int index);
	int GetCallDepth(lua_State*l);

	template<typename T>
	inline void Push(lua_State*l, const T& v)
	{
		LuaType<T>::Push(l, v);
	}

	template<typename T = LuaRef>
	inline T Get(lua_State*l, int index)
	{
		return LuaType<T>::Get(l, index);
	}

	template<typename Callable>
	int  ExceptionBoundary(lua_State*l, Callable&& func)
	{
		try
		{
			return func();
		}
		catch (const LuaException& ex)
		{
			Debug::Error(ex.what());
			luaL_error(l, ex.what());
		}
		catch (const std::exception&ex)
		{
			luaL_error(l, (std::string("Error:") + ex.what()).c_str());
		}

		return 0;
	}

	// 轻量级的UserData绑定，一般用于创建一个函数指针对象
	struct BindingUserData
	{
		template<typename T>
		static typename std::enable_if<!std::is_destructible<T>::value ||
			std::is_trivially_destructible<T>::value>::type
			PushUserdata(lua_State*l, const T& obj)
		{
			void* userdata = lua_newuserdata(l, sizeof(T));
			new(userdata) T(obj);
		}

		template<typename T>
		static typename std::enable_if<std::is_destructible<T>::value &&
			!std::is_trivially_destructible<T>::value>::type 
			PushUserdata(lua_State*l, const T& obj)
		{
			void* userdata = lua_newuserdata(l, sizeof(T));
			new(userdata) T(obj);

			PushUserdataMetatable(l);
			lua_setmetatable(l, -2);
		}

		template<typename T, typename... Args>
		static T* Create(lua_State*l, const std::string& name, Args&&... args)
		{
			T* obj = new(lua_newuserdata(l, sizeof(T)))T(std::forward<Args>(args)...);

			PushUserdataMetatable(l, name);
			lua_setmetatable(l, -2);

			return obj;
		}

		template<typename T>
		static void PushUserdataMetatable(lua_State*l, const std::string& name = "")
		{
			if (name.empty())
			{
				lua_newtable(l);
				lua_pushcfunction(l, LuaUserdataGC<T>);
				lua_setfield(l, -2, "__gc");
			}
			else
			{
				luaL_getmetatable(l, name.c_str());
				if (lua_istable(l, 1))
				{
					return;
				}
				lua_pop(l, 1);
				luaL_newmetatable(l, name.c_str());
				lua_pushcfunction(l, LuaUserdataGC<T>);
				lua_setfield(l, -2, "__gc");
			}
		}

		template<typename T>
		static int LuaUserdataGC(lua_State*l)
		{
			return LuaTools::ExceptionBoundary(l, [&] {
				if (lua_gettop(l) == 0)
				{
					LuaException::Error(l, "Gc function called without self.");
					return 0;
				}

				T* obj = static_cast<T*>(lua_touserdata(l, 1));
				if (obj == nullptr)
				{
					LuaException::Error(l, "The userdata is null when called gc.");
					return 0;
				}

				obj->~T();
				return 0;
			});
		}
	};
}
}