#include "gui/guiStage.h"
#include "helper/stringID.h"
#include "input/InputSystem.h"
#include "system/component/camera.h"
#include "system/component/light.h"
#include "system/component/object.h"
#include "system/component/terrain.h"
#include "system/component/transform.h"

#include "scripts\luaContext.h"
#include "scripts\binder\luaBinder.h"

using namespace Cjing3D;
int AutoLuaBindFunctions::REGISTER_AUTO_BINDING_FUNC(){return 0;}

void luabind_registers_AutoBindFunction(lua_State* l)
{
LuaBinder(l)
.BeginClass<CameraComponent>("CameraComponent")
.AddMethod("Update", &CameraComponent::Update)
.AddMethod("Transform", &CameraComponent::Transform)
.EndClass();

LuaBinder(l)
.BeginClass<GUIStage>("GUIStage")
.AddConstructor(_LUA_ARGS_())
.EndClass();

LuaBinder(l)
.BeginClass<InputManager>("InputManager")
.AddMethod("IsKeyDown", &InputManager::IsKeyDown)
.AddMethod("IsKeyPressed", &InputManager::IsKeyPressed)
.AddMethod("IsKeyReleased", &InputManager::IsKeyReleased)
.AddMethod("IsKeyHold", &InputManager::IsKeyHold)
.AddMethod("GetMousePos", &InputManager::GetMousePos)
.AddMethod("GetMouseWheelDelta", &InputManager::GetMouseWheelDelta)
.EndClass();

LuaBinder(l)
.BeginClass<LightComponent>("LightComponent")
.AddMethod("GetLightType", &LightComponent::GetLightType)
.AddMethod("SetLightType", &LightComponent::SetLightType)
.AddMethod("SetRange", &LightComponent::SetRange)
.AddMethod("SetEnergy", &LightComponent::SetEnergy)
.AddMethod("SetColor", &LightComponent::SetColor)
.AddMethod("SetShadowBias", &LightComponent::SetShadowBias)
.EndClass();

LuaBinder(l)
.BeginClass<ObjectComponent>("ObjectComponent")
.AddMethod("IsRenderable", &ObjectComponent::IsRenderable)
.AddMethod("SetRenderable", &ObjectComponent::SetRenderable)
.EndClass();

LuaBinder(l)
.BeginClass<StringID>("StringID")
.AddConstructor(_LUA_ARGS_(std::string&))
.EndClass();

LuaBinder(l)
.BeginClass<TerrainComponent>("TerrainComponent")
.EndClass();

LuaBinder(l)
.BeginClass<TransformComponent>("Transform")
.AddMethod("Update", &TransformComponent::Update)
.AddMethod("Clear", &TransformComponent::Clear)
.AddMethod("Translate", &TransformComponent::Translate)
.AddMethod("RotateRollPitchYaw", &TransformComponent::RotateRollPitchYaw)
.AddMethod("Scale", &TransformComponent::Scale)
.AddMethod("Rotate", &TransformComponent::Rotate)
.AddMethod("GetRotationLocal", &TransformComponent::GetRotationLocal)
.AddMethod("GetTranslationLocal", &TransformComponent::GetTranslationLocal)
.AddMethod("GetScaleLocal", &TransformComponent::GetScaleLocal)
.EndClass();

	
}
LUA_FUNCTION_AUTO_BINDER(luabind_registers_auto_binder,luabind_registers_AutoBindFunction)