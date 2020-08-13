#pragma once

#include "scripts\binder\luaBinder.h"
#include "scripts\binder\luaTypeMapping.h"
#include "core\systemContext.hpp"
#include "gui\guiStage.h"

namespace Cjing3D
{
	namespace LuaApi
	{
		void BindGUIModules(lua_State* l);
	}
}