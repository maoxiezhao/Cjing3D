#include "luaApi.h"
#include "input\InputSystem.h"
#include "core\globalContext.hpp"

namespace Cjing3D {
namespace LuaApi {

	int GetInputManager(lua_State* l)
	{
		return LuaTools::ExceptionBoundary(l, [&] {
			InputManager& inputManager = GlobalGetSubSystem<InputManager>();
			LuaTools::Push<InputManager&>(l, inputManager);

			return 1;
		});
	}

	int GetEngineDeltaTime(lua_State* l)
	{
		return LuaTools::ExceptionBoundary(l, [&] {
			F32 deltaTime = GlobalGetDeltaTime();
			LuaTools::Push<F32>(l, deltaTime);

			return 1;
		});
	}

	void BindSystemModules(lua_State* l)
	{
		LuaBinder(l)
			.BeginModule("System")
			.AddLuaCFunction("GetInputManager", GetInputManager)
			.AddLuaCFunction("GetDeltaTime", GetEngineDeltaTime)
			.EndModule();
	}
}
}