#pragma once

#include "scripts\luaBinder.h"

namespace Cjing3D
{
namespace LuaApi
{
	void BindAllModules(lua_State* l);
	void BindSystemModules(lua_State* l);
	void BindLoggerModules(lua_State* l);
}
}