#include "luaApi.h"
#include "renderer\renderer.h"
#include "system\component\camera.h"
#include "core\systemContext.hpp"

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
				F32 offsetX = LuaTools::Get<F32>(l, 3);
				F32 offsetY = LuaTools::Get<F32>(l, 4);

				Renderer::SetDebugGridSize({ col, row }, { offsetX, offsetY });
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