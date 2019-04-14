#include "luaRef.h"

namespace Cjing3D
{

LuaRef LuaRef::NULL_REF;

LuaRef::LuaRef() :
	l(nullptr),
	mRef(LUA_REFNIL)
{
}

LuaRef::LuaRef(lua_State * l, int ref) :
	l(l),
	mRef(ref)
{
}

LuaRef::LuaRef(const LuaRef& other) :
	l(nullptr),
	mRef(LUA_REFNIL)
{
	*this = other;
}

LuaRef::LuaRef(LuaRef && other) :
	l(other.l),
	mRef(other.mRef)
{
	other.l = nullptr;
	other.mRef = LUA_REFNIL;
}

/**
*	\brief ��ֵ���캯��
*
*	��Ϊluaref�������뱣��Ψһ�ԣ����ｫotherӦ�õ�Luaֵ����ѹջ������lua_ref�����µ�����
*/
LuaRef & LuaRef::operator=(const LuaRef& other)
{
	Clear();
	l = other.l;
	if (l != nullptr)
	{
		if (other.mRef == LUA_REFNIL || other.mRef == LUA_NOREF)
		{
			mRef = other.mRef;
		}
		else
		{
			lua_rawgeti(l, LUA_REGISTRYINDEX, other.mRef);
			mRef = luaL_ref(l, LUA_REGISTRYINDEX);
		}
	}

	return *this;
}

LuaRef & LuaRef::operator=(LuaRef & other)
{
	Clear();

	l = other.l;
	mRef = other.mRef;
	other.l = nullptr;
	other.mRef = LUA_REFNIL;

	return *this;
}

LuaRef::~LuaRef()
{
	Clear();
}

bool LuaRef::operator==(const LuaRef & ref) const
{
	Push();
	ref.Push();
	bool result = lua_compare(l, -1, -2, LUA_OPEQ) != 0;
	lua_pop(l, 2);
	return result;
}

bool LuaRef::operator!=(const LuaRef & ref) const
{
	return !(*this == ref);
}

LuaRef LuaRef::CreateRef(lua_State*l)
{
	return LuaRef(l, luaL_ref(l, LUA_REGISTRYINDEX));
}

LuaRef LuaRef::CreateRef(lua_State*l, int index)
{
	lua_pushvalue(l, index);
	return LuaRef(l, luaL_ref(l, LUA_REGISTRYINDEX));
}

LuaRef LuaRef::CreateTable(lua_State* l, int narray, int nrec)
{
	lua_createtable(l, narray, nrec);
	return CreateRef(l);
}

LuaRef LuaRef::CreateGlobalRef(lua_State* l)
{
	lua_pushglobaltable(l);
	return CreateRef(l);
}

LuaRef LuaRef::CreateRefFromPtr(lua_State * l, void * ptr)
{
	lua_pushlightuserdata(l, ptr);
	return CreateRef(l);
}

bool LuaRef::IsEmpty() const
{
	return l == nullptr || (mRef == LUA_REFNIL || mRef == LUA_NOREF);
}

int LuaRef::GetRef() const
{
	return mRef;
}

lua_State * LuaRef::GetLuaState() const
{
	return l;
}

void LuaRef::Clear()
{
	if (l != nullptr)
		luaL_unref(l, LUA_REGISTRYINDEX, mRef);

	l = nullptr;
	mRef = LUA_REFNIL;
}

void LuaRef::Push() const
{
	if (IsEmpty()) {
		return;
	}

	lua_rawgeti(l, LUA_REGISTRYINDEX, mRef);
}

void LuaRef::SetMetatable(LuaRef& luaRef)
{
	Push();
	luaRef.Push();
	lua_setmetatable(l, -2);
	lua_pop(l, 1);
}

}