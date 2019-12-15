#include "luaApi.h"
#include "sceneApi.h"
#include "renderer\renderer.h"
#include "system\component\camera.h"
#include "core\systemContext.hpp"

namespace Cjing3D {
	namespace LuaApi {

		int BindLuaScene::GetMainScene(lua_State* l)
		{
			return LuaTools::ExceptionBoundary(l, [&] {
				Renderer& renderer = SystemContext::GetSystemContext().GetSubSystem<Renderer>();
				Scene& scene = renderer.GetMainScene();
				BindLuaScene bindLuaScene(scene);

				LuaTools::Push<BindLuaScene>(l, bindLuaScene);
				return 1;
			});
		}

		ECS::Entity BindLuaScene::CreateEntity()
		{
			return ECS::CreateEntity();
		}

		LightComponent& BindLuaScene::CreateLight(ECS::Entity entity)
		{
			return mScene.GetOrCreateLightByEntity(entity);
		}

		TransformComponent & BindLuaScene::CreateTransform(ECS::Entity entity)
		{
			return mScene.GetOrCreateTransformByEntity(entity);
		}

		void BindSceneModules(lua_State* l)
		{
			LuaBinder(l)
				.BeginClass<BindLuaScene>("Scene")
				.AddCFunction("GetMainScene", BindLuaScene::GetMainScene)
				.AddMethod("CreateEntity", &BindLuaScene::CreateEntity)
				.AddMethod("CreateLight", &BindLuaScene::CreateLight)
				.AddMethod("CreateTransform", &BindLuaScene::CreateTransform)
				.EndClass();
		}
	}
}