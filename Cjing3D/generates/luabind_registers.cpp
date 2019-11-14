#include "input/InputSystem.h"
#include "system/component/camera.h"
#include "system/component/transform.h"

#include "scripts\luaContext.h"
#include "scripts\luaBinder.h"

using namespace Cjing3D;
int AutoLuaBindFunctions::REGISTER_AUTO_BINDING_FUNC(){return 0;}

void luabind_registers_AutoBindFunction(lua_State* l)
{
LuaBinder(l)
.BeginClass<CameraComponent>("CameraComponent")
.AddConstructor(_LUA_ARGS_())
.AddMethod("Update", &CameraComponent::Update)
.AddMethod("Transform", &CameraComponent::Transform)
.EndClass();

LuaBinder(l)
.BeginClass<InputManager>("InputManager")
.AddConstructor(_LUA_ARGS_(SystemContext&))
.AddMethod("IsKeyDown", &InputManager::IsKeyDown)
.AddMethod("IsKeyUp", &InputManager::IsKeyUp)
.AddMethod("IsKeyHold", &InputManager::IsKeyHold)
.EndClass();

LuaBinder(l)
.BeginClass<TransformComponent>("Transform")
.AddConstructor(_LUA_ARGS_())
.AddMethod("Update", &TransformComponent::Update)
.AddMethod("Clear", &TransformComponent::Clear)
.AddMethod("Translate", &TransformComponent::Translate)
.AddMethod("RotateRollPitchYaw", &TransformComponent::RotateRollPitchYaw)
.AddMethod("Scale", &TransformComponent::Scale)
.EndClass();

	
}
LUA_FUNCTION_AUTO_BINDER(luabind_registers_auto_binder,luabind_registers_AutoBindFunction)