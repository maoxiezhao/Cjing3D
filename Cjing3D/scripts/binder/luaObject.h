#pragma once

#include "scripts\binder\luaRef.h"
#include "scripts\binder\luaTools.h"
#include "scripts\binder\luaInvoke.h"
#include "scripts\binder\luaArg.h"

namespace Cjing3D
{
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

		// ����userdata��ͬʱ����metatable
		template<typename T>
		static void* Allocate(lua_State*l, void* classID)
		{
			void* mem = lua_newuserdata(l, sizeof(T));
			lua_rawgetp(l, LUA_REGISTRYINDEX, classID);
			lua_setmetatable(l, -2);
			return mem;
		}

		virtual bool IsSharedPtr()const { return false; }

		// __index and __newindex
		static bool CheckLuaObjectMetatableValid(lua_State* l, int index);
		static int MetaFuncIndex(lua_State* l);
		static int MetaFuncNewIndex(lua_State* l);

	private:
		static LuaObject* GetLuaObject(lua_State*l, int index, void* classID);
	};

	// ������Lua��������������Ҳ��Lua��������GCʱ����ö������������
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

		// ����Args����һ���µĶ�����Lua�����__call�����е���
		template<typename... Args>
		static void Push(lua_State* l, std::tuple<Args...>& args)
		{
			void* classID = ObjectIDGenerator<T>::GetID();
			void *mem = Allocate<LuaHandleObject<T>>(l, classID);
			LuaHandleObject<T>* obj = new(mem) LuaHandleObject<T>();
			ClassStructorCaller<T>::Call(obj->GetObjectPtr(), args);
		}

		// ��CPP�˵��ã����ݿ������캯����������ѹջ
		// �������BindClass
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

		virtual bool IsSharedPtr()const { return false; }

	private:
		// ����Ҫ���ǵ��������ڴ��ж���ģʽ
		// ������alignof��void*)ʱ���ض���ָ�룬��С��ʱ�����ǻ������ͣ��򲿷ְ����������͵Ľṹ�壩
		/*using ALIGN_TPYP = std::conditional<alignof(T) <= alignof(void*), T, void*>::type;
		static constexpr int MEM_PADDING = alignof(T) <= alignof(ALIGN_TPYP) ? 0 : alignof(T) - alignof(ALIGN_TPYP) + 1;*/
		unsigned char mData[sizeof(T)];
	};

	// ָ����������C++����������������C++��������GCʱ����ö������������
	// �����������õķ�ʽ����Lua��ʱ��ǿ��Ϊvoidָ�뱣��
	template<typename T>
	class LuaObjectPtr : public LuaObject
	{
	public:
		explicit LuaObjectPtr(void* obj) :
			mObjectPtr(obj)
		{
		}

		// ָ����󲻻ᴴ���µ�ʵ������ֻ�Ǵ���һ���µ�LuaObjectPtrָ�����
		static void Push(lua_State* l,  T* obj)
		{
			void* classID = ObjectIDGenerator<T>::GetID();
			void *mem = Allocate<LuaObjectPtr<T>>(l, classID);
			new(mem) LuaObjectPtr<T>(obj);
		}

		virtual void* GetObjectPtr()
		{
			return mObjectPtr;
		}

		virtual bool IsSharedPtr()const { return false; }

	private:
		void* mObjectPtr = nullptr;
	};


	// ָ����C++��������lua��C++ͬʱ��shared_ptr��ʽ����
	template<typename SP, typename T>
	class LuaObjectSharedPtr : public LuaObject
	{
	public:
		explicit LuaObjectSharedPtr(T* obj) :
			mSharedPtr(obj)
		{
		}

		explicit LuaObjectSharedPtr(const SP& sp) :
			mSharedPtr(sp)
		{
		}

		// ָ����󲻻ᴴ���µ�ʵ������ֻ�Ǵ���һ���µ�LuaObjectPtrָ�����
		static void Push(lua_State* l, T* obj)
		{
			void* classID = ObjectIDGenerator<T>::GetID();
			void* mem = Allocate<LuaObjectSharedPtr<SP, T>>(l, classID);
			new(mem) LuaObjectSharedPtr<SP, T>(obj);
		}

		static void Push(lua_State* l, const SP& sp)
		{
			void* classID = ObjectIDGenerator<T>::GetID();
			void* mem = Allocate<LuaObjectSharedPtr<SP, T>>(l, classID);
			new(mem) LuaObjectSharedPtr<SP, T>(sp);
		}

		virtual void* GetObjectPtr()
		{
			return (T*)(&(*mSharedPtr));
		}

		SP& GetSharedPtr()
		{
			return mSharedPtr;
		}

		virtual bool IsSharedPtr()const { return true; }

	private:
		SP mSharedPtr;
	};

	///////////////////////////////////////////////////////////////////////////

	// ��ѹջ�Ķ�����Ҫ�ж϶����Ƿ���ref
	template<typename SP, typename T, bool IsRef, bool IsShared>
	struct LuaObjectHandler {};

	// ��ref������ֱ�ӿ�������һ���µ�ʵ��
	template<typename T>
	struct LuaObjectHandler<T, T, false, false>
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

	// ref�����򴴽�һ��LuaPtrObject����ѹջ
	template<typename T>
	struct LuaObjectHandler<T, T, true, false>
	{
		static void Push(lua_State*l, const T&value)
		{
			LuaObjectPtr<T>::Push(l, const_cast<T*>(&value));
		}

		static T& Get(lua_State*l, int index)
		{
			LuaObject* obj = LuaObject::GetLuaObject<T>(l, index);
			return *static_cast<T*>(obj->GetObjectPtr());
		}
	};

	// shared�����򴴽�һ��LuaObjectSharedPtr����ѹջ
	template<typename SP, typename T>
	struct LuaObjectHandler<SP, T, true, true>
	{
		static void Push(lua_State* l, const SP& value)
		{
			LuaObjectSharedPtr<SP, T>::Push(l, value);
		}

		static SP& Get(lua_State* l, int index)
		{
			LuaObject* obj = LuaObject::GetLuaObject<T>(l, index);
			if (obj == nullptr || obj->IsSharedPtr() == false) {
				LuaException::Error(l, "Failed to get object and is not shared ptr.");
				return SP();
			}
			return static_cast<LuaObjectSharedPtr<SP, T>*>(obj)->GetSharedPtr();
		}
	};

	///////////////////////////////////////////////////////////////////////////

	template<typename T>
	struct LuaTypeClassTraits
	{
		using ObjectType = T;
		static constexpr bool IsShared = false;
	};

	template<typename T>
	struct LuaTypeClassTraits<std::shared_ptr<T>>
	{
		using ObjectType = std::remove_cv_t<T>;

		static constexpr bool IsShared = true;
	};

	template<typename T, bool IS_REF>
	struct LuaTypeClassMapping
	{
		using ObjectType = typename LuaTypeClassTraits<T>::ObjectType;
		static constexpr bool IsShared = LuaTypeClassTraits<T>::IsShared;
		static constexpr bool IsRef = IsShared ? true : IS_REF;

		static void Push(lua_State*l, const T&value)
		{
			LuaObjectHandler<T, ObjectType, IsRef, IsShared>::Push(l, value);
		}

		static T& Get(lua_State*l, int index)
		{
			return LuaObjectHandler<T, ObjectType, IsRef, IsShared>::Get(l, index);
		}

		static const T& Opt(lua_State* l, int index, const T& defValue)
		{
			return lua_isnoneornil(l, index) ? defValue : Get(l, index);
		}
	};

	template<typename T>
	struct LuaTypeNormalMapping<T*>
	{
		using Type = typename std::decay<T>::type;
		static void Push(lua_State* l, Type* value)
		{
			if (value == nullptr)
			{
				lua_pushnil(l);
			}
			else
			{
				LuaObjectPtr<T>::Push(l, const_cast<Type*>(value));
			}
		}

		static T* Get(lua_State* l, int index)
		{
			LuaObject* obj = LuaObject::GetLuaObject<T>(l, index);
			return static_cast<T*>(obj->GetObjectPtr());
		}

		static T* Opt(lua_State* l, int index, Type* defValue)
		{
			return lua_isnoneornil(l, index) ? defValue : Get(l, index);
		}
	};

	///////////////////////////////////////////////////////////////////////////

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
}