#pragma once

#include "scripts\luaBinder.h"
#include "scripts\luaTypeMapping.h"
#include "core\systemContext.hpp"

namespace Cjing3D
{
	namespace LuaApi
	{
		void BindJsonModules(lua_State* l);
	}
}