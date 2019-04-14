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

}