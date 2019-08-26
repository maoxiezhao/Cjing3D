#pragma once

#include "scripts\luaBinder.h"

namespace Cjing3D
{
namespace LuaApi
{
	void BindAllModules(lua_State* l);

	// register function
	void BindMainComponent(lua_State* l);
	void BindInputManager(lua_State* l);
	void BindSceneSystem(lua_State* l);
}
}