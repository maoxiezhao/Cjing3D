#include "luaBinder_impl.h"

#include <assert.h>

int Cjing3D::BindModuleMetaMethod::Index(lua_State * l)
{
	// t[key], arg1:t arg2:key
	// stack: t key
	lua_getmetatable(l, 1);
	lua_pushvalue(l, 2);

	// stack: t key meta key
	lua_rawget(l, -2);

	if (lua_isnil(l, -1))
	{
		// stack: t key meta nil
		lua_pop(l, 1);
		lua_pushliteral(l, "___getters");
		lua_rawget(l, -2);

		// stack: t key meta _getters_table
		lua_pushvalue(l, 2);
		lua_rawget(l, -2);

		if (lua_iscfunction(l, -1))
		{
			lua_call(l, 0, 1);
		}
	}

	return 1;
}

int Cjing3D::BindModuleMetaMethod::NewIndex(lua_State * l)
{
	// t[key] = value, arg1:t arg2:key arg3:value

	// stack: t key value
	lua_getmetatable(l, 1);
	lua_pushliteral(l, "___setters");
	lua_rawget(l, -2);
	assert(lua_istable(l, -1));

	// stack: t key value meta _setter_table
	lua_pushvalue(l, 2);
	lua_rawget(l, -2);

	// stack: t key value meta _setter_table setter
	if (lua_iscfunction(l, -1))
	{
		lua_pushvalue(l, 2);
		lua_call(l, 1, 0);
	}
	else
	{
		lua_pop(l, 3);
		// stack: t key value meta
		lua_rawset(l, 1);
	}

	return 0;
}