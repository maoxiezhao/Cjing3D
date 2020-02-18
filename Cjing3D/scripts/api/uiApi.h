#pragma once

#include "scripts\luaBinder.h"
#include "scripts\luaTypeMapping.h"
#include "core\systemContext.hpp"
#include "gui\guiStage.h"

namespace Cjing3D
{
	namespace LuaApi
	{
		void BindGUIModules(lua_State* l);
	}
}