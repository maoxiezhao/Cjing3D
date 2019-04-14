#pragma once

#include "scripts\luaRef.h"
#include "scripts\luaTools.h"
#include "scripts\luaInvoke.h"
#include "scripts\luaArg.h"

namespace Cjing3D
{
	class LuaObject;

	// 为了支持在编译期，通过AddConstructor中传入参数的不同，来
	// 实现LuaObjectConstructor::Call时通过typenam...Args来解析各个参数，
	// AddConstructor 将传入一个函数指针，参数为AddConstructor传入的参数，
	// 定义为__args(*)(__VA_ARGS__)
	struct _lua_args {};

	#define _LUA_ARGS_TYPE(...) _lua_args(*)(__VA_ARGS__) 
	#define  _LUA_ARGS_(...) static_cast<_LUA_ARGS_TYPE(__VA_ARGS__)>(nullptr)
	template<typename T, typename ARGS>
	struct LuaObjectConstructor;

	template<typename T, typename...Args>
	struct LuaObjectConstructor<T, _lua_args(*)(Args...)>
	{
		static int Call(lua_State*l)
		{
			return LuaTools::ExceptionBoundary(l, [&] {
				LuaArgValueTuple<Args...> args;
				LuaInputArgs<Args...>::Get(l, 2, args);
				LuaHandleObject<T>::Push(l, args);

				return 1;
			});
		}
	};

	template<typename T>
	struct LuaObjectDestructor
	{
		static int Call(lua_State*l)
		{
			return LuaTools::ExceptionBoundary(l, [&] {
				LuaObject* obj = LuaObject::GetLuaObject<T>(l, 1);
				if (obj != nullptr) {
					obj->~LuaObject();
				}

				return 0;
			});
		}
	};

	///////////////////////////////////////////////////////////////////////////
	// lua base object
	class LuaObject
	{
	public:
		LuaObject() {}
		virtual ~LuaObject() {}

		virtual void* GetObjectPtr()
		{
			return nullptr;
		}

		template<typename T>
		static LuaObject* GetLuaObject(lua_State*l, int index)
		{
			void* classID = ObjectIDGenerator<T>::GetID();
			return GetLuaObject(l, index, classID);
		}

		template<typename T>
		static T* GetObject(lua_State*l, int index)
		{
			void* classID = ObjectIDGenerator<T>::GetID();
			LuaObject* obj = GetLuaObject(l, index, classID);
			return obj ? static_cast<T*>(obj->GetObjectPtr()) : nullptr;
		}

		// 分配userdata，同时设置metatable
		template<typename T>
		static void* Allocate(lua_State*l, void* classID)
		{
			void* mem = lua_newuserdata(l, sizeof(T));
			lua_rawgetp(l, LUA_REGISTRYINDEX, classID);
			lua_setmetatable(l, -2);
			return mem;
		}

		// __index and __newindex
		static bool CheckLuaObjectMetatableValid(lua_State* l, int index);
		static int MetaFuncIndex(lua_State* l);
		static int MetaFuncNewIndex(lua_State* l);

	private:
		static LuaObject* GetLuaObject(lua_State*l, int index, void* classID);
	};

	// 对象由Lua创建，生命周期也由Lua来管理，当GC时会调用对象的析构函数
	template<typename T>
	class LuaHandleObject : public LuaObject
	{
	public:
		LuaHandleObject()
		{
		}

		~LuaHandleObject()
		{
			T* obj = static_cast<T*>(GetObjectPtr());
			obj->~T();
		}

		// 根据Args构造一个新的对象，由Lua发起的__call函数中调用
		template<typename... Args>
		static void Push(lua_State* l, std::tuple<Args...>& args)
		{
			void* classID = ObjectIDGenerator<T>::GetID();
			void *mem = Allocate<LuaHandleObject<T>>(l, classID);
			LuaHandleObject<T>* obj = new(mem) LuaHandleObject<T>();
			ClassStructorCaller<T>::Call(obj->GetObjectPtr(), args);
		}

		// 在CPP端调用，根据拷贝构造函数创建对象压栈
		// 对象必须BindClass
		static void Push(lua_State* l, const T& obj)
		{
			void* classID = ObjectIDGenerator<T>::GetID();
			void *mem = Allocate<LuaHandleObject<T>>(l, classID);
			LuaHandleObject<T>* lua_obj = new(mem) LuaHandleObject<T>();
			new(lua_obj->GetObjectPtr()) T(obj);
		}

		virtual void* GetObjectPtr()
		{
			return &mData[0];
		}

	private:
		// 还需要考虑到对象在内存中对齐模式
		// 当大于alignof（void*)时，必定是指针，当小于时可能是基础类型（或部分包含基础类型的结构体）
		/*using ALIGN_TPYP = std::conditional<alignof(T) <= alignof(void*), T, void*>::type;
		static constexpr int MEM_PADDING = alignof(T) <= alignof(ALIGN_TPYP) ? 0 : alignof(T) - alignof(ALIGN_TPYP) + 1;*/
		unsigned char mData[sizeof(T)];
	};

	// 指针对象必须在C++创建，生命周期由C++来管理，当GC时会调用对象的析构函数
	template<typename T>
	class LuaObjectPtr : public LuaObject
	{
	public:
		LuaObjectPtr(T& obj) :
			mObjectPtr(&ojb)
		{
		}

		// 指针对象不会创建新的实例，而只是创建一个新的LuaObjectPtr指向对象
		static void Push(lua_State* l, const T& obj)
		{
			void* classID = ObjectIDGenerator<T>::GetID();
			void *mem = Allocate<LuaObjectPtr<T>>(l, classID);
			new(mem) LuaObjectPtr<T>(obj);
		}

		virtual void* GetObjectPtr()
		{
			return mObjectPtr;
		}

	private:
		void* mObjectPtr = nullptr;
	};

	///////////////////////////////////////////////////////////////////////////

	// 对压栈的对象需要判断对象是否是ref
	template<typename T, bool IsRef>
	struct LuaObjectHandler {};

	// 非ref对象，则直接拷贝构造一个新的实例
	template<typename T>
	struct LuaObjectHandler<T, false>
	{
		static void Push(lua_State*l, const T&value)
		{
			LuaHandleObject<T>::Push(l, value);
		}

		static T& Get(lua_State*l, int index)
		{
			LuaObject* obj = LuaObject::GetLuaObject<T>(l, index);
			return *static_cast<T*>(obj->GetObjectPtr());
		}
	};

	// ref对象，则创建一个LuaPtrObject对象压栈
	template<typename T>
	struct LuaObjectHandler<T, true>
	{
		static void Push(lua_State*l, const T&value)
		{
			LuaObjectPtr<T>::Push(l, value);
		}

		static T& Get(lua_State*l, int index)
		{
			LuaObject* ojb = LuaObject::GetLuaObject<T>(l, index);
			return *static_cast<T*>(obj->GetObjectPtr());
		}
	};

	template<typename T, bool IsRef>
	struct LuaTypeClassMapping
	{
		static void Push(lua_State*l, const T&value)
		{
			LuaObjectHandler<T, IsRef>::Push(l, value);
		}

		static T& Get(lua_State*l, int index)
		{
			return LuaObjectHandler<T, IsRef>::Get(l, index);
		}
	};
}