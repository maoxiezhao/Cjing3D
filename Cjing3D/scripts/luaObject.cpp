#include "luaObject.h"

namespace Cjing3D
{
	bool LuaObject::CheckLuaObjectMetatableValid(lua_State * l, int index)
	{
		// check current class
		// 1.get current_metatable
		// 2.get classID frome metatable
		// 3.get target_metatable from lua_registryindex and compare with current_metatable

		index = LuaTools::GetPositiveIndex(l, index);
		lua_getmetatable(l, index);
		lua_rawgetp(l, -1, ObjectIDGenerator<LuaObject>::GetID());
		// stack: obj ... meta luaObjectID	
		lua_rawget(l, LUA_REGISTRYINDEX);

		bool result = (lua_rawequal(l, -1, -2));
		lua_pop(l, 2);
		return result;
	}

	int LuaObject::MetaFuncIndex(lua_State * l)
	{
		if (!CheckLuaObjectMetatableValid(l, 1))
		{
			LuaException::Error(l, "Except userdata, got ." +
				std::string(lua_typename(l, lua_type(l, 1))));
			return 1;
		}
		
		// find in metatalbe first
		lua_getmetatable(l, 1);
		lua_pushvalue(l, 2); // push key
		lua_rawget(l, -2);

		return 1;
	}

	int LuaObject::MetaFuncNewIndex(lua_State * l)
	{
		if (!CheckLuaObjectMetatableValid(l, 1))
		{
			LuaException::Error(l, "Except userdata, got ." +
				std::string(lua_typename(l, lua_type(l, 1))));
			return 1;
		}

		return 1;
	}

	LuaObject * LuaObject::GetLuaObject(lua_State * l, int index, void * classID)
	{
		index = LuaTools::GetPositiveIndex(l, index);
		if (!lua_isuserdata(l, index))
		{
			LuaException::Error(l, "Except userdata, got ." + 
				std::string(lua_typename(l, lua_type(l, index))));
			return nullptr;
		}

		lua_rawgetp(l, LUA_REGISTRYINDEX, classID);
		if (!lua_istable(l, -1))
		{
			LuaException::Error(l, "Invalid class without metatable.");
			return nullptr;
		}

		// check class
		// stack: obj args... except_meta
		lua_getmetatable(l, index);
		if (!lua_istable(l, -1))
		{
			LuaException::Error(l, "Invalid obj without metatable.");
			return nullptr;
		}

		// except_meta crrent_meta
		bool is_valid = false;
		while(!lua_isnil(l, -1))
		{
			if (lua_rawequal(l, -1, -2))
			{
				is_valid = true;
				break;
			}

			// get super class
			lua_pushstring(l, "__SUPER");
			lua_rawget(l, -2);

			// stack: except_meta current_meta nil/super_meta
			lua_remove(l, -2);
		}

		lua_pop(l, 2);

		// stack: obj args... 
		if (is_valid == true)
		{
			return static_cast<LuaObject*>(lua_touserdata(l, index));
		}
		else
		{
			return nullptr;			
		}
	}
}