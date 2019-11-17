#include "luaApi.h"
#include "input\InputSystem.h"
#include "core\systemContext.hpp"

namespace Cjing3D {
namespace LuaApi {

	int GetInputManager(lua_State* l)
	{
		return LuaTools::ExceptionBoundary(l, [&] {
			InputManager& inputManager = SystemContext::GetSystemContext().GetSubSystem<InputManager>();
			LuaTools::Push<InputManager&>(l, inputManager);

			return 1;
		});
	}

	int GetEngineDeltaTime(lua_State* l)
	{
		return LuaTools::ExceptionBoundary(l, [&] {
			EngineTime time = SystemContext::GetSystemContext().GetEngineTime();
			F32 deltaTime = F32(time.deltaTime / 1000.0f);
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