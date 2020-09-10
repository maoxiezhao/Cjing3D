#pragma once

#include "scripts\binder\luaBinder_impl.h"

#include <string>


namespace Cjing3D
{
	// LuaBindClass在bindClass时会创建一个StaticMeta和一个ClassMeta,后者在创建userdata时会setmetatable
	// 同时RegisterStaticFunction会将函数设置在StaticMeta中，而RegisterMethod会将成员函数设置在classmeta

	class LuaBindClassBase
	{
	public:
		LuaBindClassBase(lua_State* l, LuaRef& meta) :
			mLuaState(l),
			mCurrentMeta(meta)
		{}

		lua_State* GetLuaState() { return mCurrentMeta.GetLuaState(); }

	protected:
		static bool BindClassMeta(LuaRef& currentMeta, LuaRef& parentMeta, const std::string& name, void* currentClassID);
		static bool BindExtendClassMeta(LuaRef& currentMeta, LuaRef& parentMeta, const std::string& name, void* currentClassID, void* superClassID);

		void RegisterStaticFunction(const std::string& name, LuaRef func);
		void RegisterMethod(const std::string& name, LuaRef func);

		void SetMemberGetter(const std::string& name, const LuaRef& getter);
		void SetMemberSetter(const std::string& name, const LuaRef& setter);
		void SetMemberReadOnly(const std::string& name);

		static int ReadOnlyError(lua_State* l);

		lua_State * mLuaState = nullptr;
		LuaRef mCurrentMeta = LuaRef::NULL_REF;
	};

	// Binding Class
	// used for add func, add member
	template<typename T, typename ParentT>
	class LuaBindClass : public LuaBindClassBase
	{
	public:
		static LuaBindClass<T, ParentT> BindClass(lua_State* l, LuaRef& parentMeta, const std::string& name)
		{
			LuaRef currentMeta;
			if (BindClassMeta(currentMeta, parentMeta, name, ObjectIDGenerator<T>::GetID()))
			{
				currentMeta.RawGet("__CLASS").RawSet("__gc", &LuaObjectDestructor<T>::Call);
			}

			return LuaBindClass<T, ParentT>(l, currentMeta);
		}

		template<typename SUPER>
		static LuaBindClass<T, ParentT> BindExtendClass(lua_State* l, LuaRef& parentMeta, const std::string& name)
		{
			LuaRef currentMeta;
			if (BindExtendClassMeta(currentMeta, parentMeta, name, ObjectIDGenerator<T>::GetID(), ObjectIDGenerator<SUPER>::GetID()))
			{
				currentMeta.RawGet("__CLASS").RawSet("__gc", &LuaObjectDestructor<T>::Call);
			}

			return LuaBindClass<T, ParentT>(l, currentMeta);
		}

		template<typename ARGS>
		LuaBindClass<T, ParentT>& AddConstructor(ARGS)
		{
			// 1. local obj = CLAZZ(...);
			// 2. local obj = CLAZZ:new(...);

			mCurrentMeta.RawSet("__call", LuaObjectConstructor<T, ARGS>::Call);
			mCurrentMeta.RawSet("new", LuaObjectConstructor<T, ARGS>::Call);

			return *this;
		}

		LuaBindClass<T, ParentT>& AddCFunctionConstructor(FunctionExportToLua function)
		{
			// 1. local obj = CLAZZ(...);
			// 2. local obj = CLAZZ:new(...);

			mCurrentMeta.RawSet("__call", function);
			mCurrentMeta.RawSet("new", function);

			return *this;
		}

		LuaBindClass<T, ParentT>& AddMetaFunction(const std::string& name, FunctionExportToLua function)
		{
			mCurrentMeta.RawGet("__CLASS").RawSet(name, function);

			return *this;
		}

		LuaBindClass<T, ParentT>& AddCFunction(const std::string& name, FunctionExportToLua function)
		{
			mCurrentMeta.RawSet(name, function);

			return *this;
		}

		template<typename FUNC>
		LuaBindClass<T, ParentT>& AddFunction(const std::string& name, const FUNC& func)
		{
			using FunctionCaller = BindClassStaticFunc<FUNC>;
			LuaRef funcRef = LuaRef::CreateFuncWithUserdata(mLuaState, &FunctionCaller::Caller, func);
			RegisterStaticFunction(name, funcRef);

			return *this;
		}

		template<typename FUNC>
		LuaBindClass<T, ParentT>& AddMethod(const std::string& name, const FUNC& func)
		{
			using MethodCaller = BindClassMethodFunc<T, FUNC>;
			LuaRef funcRef = LuaRef::CreateFuncWithUserdata(mLuaState, &MethodCaller::Caller, func);
			RegisterMethod(name, funcRef);

			return *this;
		}

		LuaBindClass<T, ParentT>& AddCFunctionMethod(const std::string& name, FunctionExportToLua function)
		{
			mCurrentMeta.RawGet("__CLASS").RawSet(name, function);
			return *this;
		}

		template<typename V>
		LuaBindClass<T, ParentT>& AddMember(const std::string& name, V T::* pointer)
		{
			lua_State* l = GetLuaState();
			SetMemberGetter(name, LuaRef::CreateFunc(l, &BindClassMemberMethod<T, V>::Getter, pointer));
			SetMemberSetter(name, LuaRef::CreateFunc(l, &BindClassMemberMethod<T, V>::Setter, pointer));

			return *this;
		}

		template<typename V>
		LuaBindClass<T, ParentT>& AddConstMember(const std::string& name, V T::* pointer)
		{
			lua_State* l = GetLuaState();
			SetMemberGetter(name, LuaRef::CreateFunc(l, &BindClassMemberMethod<T, V>::Getter, &pointer));
			SetMemberReadOnly(name);

			return *this;
		}

		ParentT EndClass()
		{
			return ParentT(mLuaState);
		}

	private:
		LuaBindClass(lua_State* l, LuaRef& meta) :
			LuaBindClassBase(l, meta)
		{}
	};

	// Binding Module
	class LuaBindModuleBase
	{
	public:
		LuaBindModuleBase(LuaRef& meta) :
			mCurrentMeta(meta)
		{}

		LuaBindModuleBase(LuaRef& meta, const std::string& name);

		static int ReadOnlyError(lua_State* l);
		lua_State* GetLuaState() { return mCurrentMeta.GetLuaState(); }

	protected:
		void SetGetter(const std::string& name, const LuaRef& getter);
		void SetSetter(const std::string& name, const LuaRef& setter);
		void SetReadOnly(const std::string& name);

		LuaRef mCurrentMeta = LuaRef::NULL_REF;
		std::string mName;
	};

	template<typename ParentT>
	class LuaBindModule : public LuaBindModuleBase
	{
	public:
		LuaBindModule(LuaRef& meta, const std::string& name) :
			LuaBindModuleBase(meta, name),
			mName(name)
		{}

		LuaBindModule<ParentT>& AddEnum(const std::string& name, int value)
		{
			return AddConstant(name, value);
		}

		template<typename V>
		LuaBindModule<ParentT>& AddConstant(const std::string& name, const V& value)
		{
			lua_State* l = GetLuaState();
			LuaRef ref = LuaRef::CreateRefFromValue(l, value);
			if (ref.IsFunction()) {
				// TODO
			}

			SetGetter(name, ref);
			SetReadOnly(name);
			return *this;
		}

		template<typename V>
		LuaBindModule<ParentT>& AddVariable(const std::string& name, const V& value)
		{
			lua_State* l = GetLuaState();
			SetSetter(name, LuaRef::CreateFuncWithPtr(l, &BindVariableMethod<V>::Setter, &value));
			SetGetter(name, LuaRef::CreateFuncWithPtr(l, &BindVariableMethod<V>::Getter, &value));
			return *this;
		}

		template<typename FUNC>
		LuaBindModule<ParentT>& AddFunction(const std::string& name, const FUNC& func)
		{
			using FunctionCaller = BindClassStaticFunc<FUNC>;
			LuaRef funcRef = LuaRef::CreateFunc(GetLuaState(), &FunctionCaller::Caller, FunctionCaller::ConvertToFunction(func));
			mCurrentMeta.RawSet(name, funcRef);

			return *this;
		}

		LuaBindModule<ParentT>& AddLuaCFunction(const std::string& name, lua_CFunction function)
		{
			mCurrentMeta.RawSet(name, function);

			return *this;
		}

		template<typename T>
		LuaBindClass<T, LuaBindModule<ParentT>> BeginClass(const std::string& name)
		{
			return LuaBindClass<T, LuaBindModule<ParentT>>::BindClass(GetLuaState(), mCurrentMeta, name);
		}

		LuaBindModule<LuaBindModule<ParentT>> BeginModule(const std::string& name)
		{
			return LuaBindModule<LuaBindModule<ParentT>>(GetLuaState(), mCurrentMeta, name);
		}

		ParentT EndModule()
		{
			return ParentT(mCurrentMeta.GetLuaState());
		}

	protected:
		std::string mName;
	};

	// Lua binder main class
	class LuaBinder
	{
	public:
		LuaBinder(lua_State* l) :
			mLuaState(l)
		{
			mCurrentMeta = LuaRef::CreateGlobalRef(l);
		}

		template<typename T>
		LuaBindClass<T, LuaBinder> BeginClass(const std::string& name)
		{
			return LuaBindClass<T, LuaBinder>::BindClass(mLuaState, mCurrentMeta, name);
		}

		template<typename T, typename SUPER>
		LuaBindClass<T, LuaBinder> BeginExtendClass(const std::string& name)
		{
			return LuaBindClass<T, LuaBinder>::template BindExtendClass<SUPER>(mLuaState, mCurrentMeta, name);
		}

		LuaBindModule<LuaBinder> BeginModule(const std::string& name)
		{
			return LuaBindModule<LuaBinder>(mCurrentMeta, name);
		}

		LuaBinder& AddGlobalCFunction(const std::string& name, lua_CFunction function)
		{
			mCurrentMeta.RawSet(name, function);
			return *this;
		}

	private:
		lua_State * mLuaState = nullptr;
		LuaRef mCurrentMeta = LuaRef::NULL_REF;
	};

	class AutoLuaBindFunctions
	{
	public:
		static AutoLuaBindFunctions& GetInstance()
		{
			static AutoLuaBindFunctions funcs;
			return funcs;
		}

		void PushAutoBindFunction(std::function<void(lua_State* l)> func);
		void DoAutoBindFunctions(lua_State* l);

		std::vector<std::function<void(lua_State* l)>> mfuncs;

		static int REGISTER_AUTO_BINDING_FUNC();
	};

#define LUA_FUNCTION_AUTO_BINDER(LuaName, function) \
	class AutoBinder##LuaName{				\
		public:AutoBinder##LuaName(){		\
			AutoLuaBindFunctions::GetInstance().PushAutoBindFunction(function); }	\
		static AutoBinder##LuaName instance;};	\
	 AutoBinder##LuaName  AutoBinder##LuaName::instance;
}