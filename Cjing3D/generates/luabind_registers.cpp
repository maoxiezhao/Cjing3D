#include "input/InputSystem.h"
#include "scripts\luaContext.h"
#include "scripts\luaBinder.h"

using namespace Cjing3D;
int AutoLuaBindFunctions::REGISTER_AUTO_BINDING_FUNC(){return 0;}

void luabind_registers_AutoBindFunction(lua_State* l)
{
LuaBinder(l)
.BeginClass<InputManager>("InputManager")
.AddConstructor(_LUA_ARGS_(SystemContext&))
.AddMethod("IsKeyDown", &InputManager::IsKeyDown)
.AddMethod("IsKeyUp", &InputManager::IsKeyUp)
.AddMethod("IsKeyHold", &InputManager::IsKeyHold)
.EndClass();

	
}
LUA_FUNCTION_AUTO_BINDER(luabind_registers_auto_binder,luabind_registers_AutoBindFunction)