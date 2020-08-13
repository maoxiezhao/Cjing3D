#pragma once

#include "scripts\binder\luaBinder.h"
#include "scripts\binder\luaTypeMapping.h"
#include "core\systemContext.hpp"

namespace Cjing3D
{
	namespace LuaApi
	{
		void BindJsonModules(lua_State* l);
	}
}