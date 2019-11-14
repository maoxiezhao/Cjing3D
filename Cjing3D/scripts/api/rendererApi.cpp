#include "luaApi.h"
#include "renderer\renderer.h"
#include "system\sceneSystem.h"
#include "system\component\camera.h"
#include "core\systemContext.hpp"

namespace Cjing3D {
	namespace LuaApi {

		int GetMainCamera(lua_State* l)
		{
			return LuaTools::ExceptionBoundary(l, [&] {
				Renderer& renderer = SystemContext::GetSystemContext().GetSubSystem<Renderer>();
				CameraPtr camera = renderer.GetCamera();
				if (camera == nullptr) {
					return 0;
				}

				LuaTools::Push<CameraComponent&>(l, *camera);
				return 1;
			});
		}

		void BindRenderModules(lua_State* l)
		{
			LuaBinder(l)
				.BeginModule("Render")
				.AddLuaCFunction("GetMainCamera", GetMainCamera)
				.EndModule();
		}
	}
}