
#include "scripts\luaContext.h"
#include "scripts\luaBinder.h"

using namespace Cjing3D;

void luabind_registers_AutoBindFunction(lua_State* l)
{
	
}
LUA_FUNCTION_AUTO_BINDER(luabind_registers_auto_binder,luabind_registers_AutoBindFunction)