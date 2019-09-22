#include "luaBinder.h"

namespace Cjing3D {

	bool LuaBindClassBase::BindClassMeta(LuaRef & currentMeta, LuaRef & parentMeta, const std::string & name, void * currentClassID)
	{
		LuaRef ref = parentMeta.RawGet(name);
		if (!ref.IsEmpty())
		{
			currentMeta = ref;
			return false;
		}

		lua_State* l = parentMeta.GetLuaState();

		// class id
		LuaRef classIDRef = LuaRef::CreateRefFromPtr(l, currentClassID);

		// create class metatable
		LuaRef classTable = LuaRef::CreateTable(l);
		classTable.SetMetatable(classTable);
		classTable.RawSet("__index", &LuaObject::MetaFuncIndex);
		classTable.RawSet("__newindex", &LuaObject::MetaFuncNewIndex);
		classTable.RawSetp(ObjectIDGenerator<LuaObject>::GetID(), classIDRef);

		LuaRef staticClassTable = LuaRef::CreateTable(l);
		staticClassTable.RawSet("__CLASS", classTable);

		// rawset metatable into registryIndex
		LuaRef registry = LuaRef::CreateRef(l, LUA_REGISTRYINDEX);
		registry.RawSet(classIDRef, classTable);

		// no use now.
		LuaRef constClassTable = LuaRef::CreateTable(l);
		constClassTable.SetMetatable(constClassTable);
		staticClassTable.RawSet("__CONST", constClassTable);

		parentMeta.RawSet(name, staticClassTable);
		currentMeta = staticClassTable;

		return true;
	}

	void LuaBindClassBase::RegisterStaticFunction(const std::string & name, LuaRef func)
	{
		mCurrentMeta.RawSet(name, func);
	}

	void LuaBindClassBase::RegisterMethod(const std::string & name, LuaRef func)
	{
		LuaRef metaClass = mCurrentMeta.RawGet("__CLASS");
		metaClass.RawSet(name, func);
	}

	LuaBindModuleBase::LuaBindModuleBase(LuaRef & meta, const std::string & name)
	{
		LuaRef ref = meta.RawGet(name);
		if (!ref.IsEmpty())
		{
			mCurrentMeta = meta;
			return;
		}

		lua_State* l = meta.GetLuaState();

		std::string typeName = "<Module " + name + ">";
		LuaRef moduleTable = LuaRef::CreateTable(l);
		moduleTable.SetMetatable(moduleTable);

		moduleTable.RawSet("__index", &BindModuleMetaMethod::Index);
		moduleTable.RawSet("__newindex", &BindModuleMetaMethod::NewIndex);
		moduleTable.RawSet("___getters", LuaRef::CreateTable(l));
		moduleTable.RawSet("___setters", LuaRef::CreateTable(l));
		moduleTable.RawSet("___type", typeName);

		meta.RawSet(name, moduleTable);
		mCurrentMeta = moduleTable;
	}

	int LuaBindModuleBase::ReadOnlyError(lua_State* l)
	{
		const std::string name = lua_tostring(l, lua_upvalueindex(1));
		LuaException::Error(l, "The variable " + name + " is read only.\n");
		return 0;
	}

	void LuaBindModuleBase::SetGetter(const std::string & name, const LuaRef & getter)
	{
		mCurrentMeta.RawGet("___getters").RawSet(name, getter);
	}

	void LuaBindModuleBase::SetSetter(const std::string& name, const LuaRef& setter)
	{
		mCurrentMeta.RawGet("___setters").RawSet(name, setter);
	}

	void LuaBindModuleBase::SetReadOnly(const std::string & name)
	{
		std::string fullName = mCurrentMeta.RawGet<std::string>("___type") + "." + name;
		lua_State* l = mCurrentMeta.GetLuaState();

		SetSetter(name, LuaRef::CreateFunctionWithArgs(l, LuaBindModuleBase::ReadOnlyError, fullName));
	}

	void AutoLuaBindFunctions::PushAutoBindFunction(std::function<void(lua_State*l)> func)
	{
		mfuncs.push_back(func);
	}

	void AutoLuaBindFunctions::DoAutoBindFunctions(lua_State* l)
	{
		for (auto func : mfuncs) {
			func(l);
		}
	}

}