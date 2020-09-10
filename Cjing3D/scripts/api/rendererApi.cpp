#include "luaApi.h"
#include "renderer\renderer.h"
#include "system\component\camera.h"
#include "core\globalContext.hpp"

namespace Cjing3D {
	namespace LuaApi {

		int GetMainCamera(lua_State* l)
		{
			return LuaTools::ExceptionBoundary(l, [&] {
				CameraComponent& camera = Renderer::GetCamera();

				LuaTools::Push<CameraComponent&>(l, camera);
				return 1;
			});
		}

		int SetDebugGrid(lua_State* l)
		{
			return LuaTools::ExceptionBoundary(l, [&] {
				I32 col = LuaTools::Get<I32>(l, 1);
				I32 row = LuaTools::Get<I32>(l, 2);

				Renderer::SetDebugGridSize({ col, row });
				return 0;
			});
		}

		void BindRenderModules(lua_State* l)
		{
			LuaBinder(l)
				.BeginModule("Render")
				.AddLuaCFunction("GetMainCamera", GetMainCamera)
				.AddLuaCFunction("SetDebugGrid", SetDebugGrid)
				.EndModule();
		}
	}
}