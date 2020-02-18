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
		l(other.l),
		mRef(LUA_REFNIL)
	{
		if (l != nullptr && !other.IsEmpty())
		{
			lua_rawgeti(l, LUA_REGISTRYINDEX, other.mRef);
			mRef = luaL_ref(l, LUA_REGISTRYINDEX);
		}
	}

	LuaRef::LuaRef(LuaRef && other) :
		l(other.l),
		mRef(other.mRef)
	{
		other.mRef = LUA_REFNIL;
	}

	/**
	*	\brief ��ֵ���캯��
	*
	*	��Ϊluaref�������뱣��Ψһ�ԣ����ｫotherӦ�õ�Luaֵ����ѹջ������lua_ref�����µ�����
	*/
	LuaRef & LuaRef::operator=(const LuaRef& other)
	{
		if (this != &other) 
		{
			Clear();

			l = other.l;

			if (l != nullptr)
			{
				lua_rawgeti(l, LUA_REGISTRYINDEX, other.mRef);
				mRef = luaL_ref(l, LUA_REGISTRYINDEX);
			}
		}

		return *this;
	}

	LuaRef & LuaRef::operator=(LuaRef&& other)
	{
		std::swap(l, other.l);
		std::swap(mRef, other.mRef);

		return *this;
	}

	LuaRef::~LuaRef()
	{
		Clear();
	}

	bool LuaRef::operator==(const LuaRef & ref) const
	{
		if (GetRef() == ref.GetRef()) {
			return true;
		}

		if (IsEmpty() || ref.IsEmpty()) {
			return false;
		}

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

	bool LuaRef::IsRefEmpty() const
	{
		if (IsEmpty()) {
			return true;
		}

		bool ret = true;
		lua_rawgeti(l, LUA_REGISTRYINDEX, mRef);
		ret = lua_isnil(l, -1);
		lua_pop(l, 1);

		return ret;
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
		if (l != nullptr && mRef != LUA_REFNIL && mRef != LUA_NOREF) {
			luaL_unref(l, LUA_REGISTRYINDEX, mRef);
		}

		l = nullptr;
		mRef = LUA_REFNIL;
	}

	bool LuaRef::IsFunction() const
	{
		if (IsEmpty()) {
			return false;
		}

		Push();
		int refType = lua_type(l, -1);
		lua_pop(l, 1);

		return refType == LUA_TFUNCTION;
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