#include "luaApi.h"
#include "sceneApi.h"
#include "renderer\renderer.h"
#include "system\component\camera.h"
#include "core\globalContext.hpp"
#include "input\InputSystem.h"

namespace Cjing3D {
	namespace LuaApi {

		int BindLuaScene::GetMainScene(lua_State* l)
		{
			return LuaTools::ExceptionBoundary(l, [&] {
				Scene& scene = Scene::GetScene();
				BindLuaScene bindLuaScene(scene);

				LuaTools::Push<BindLuaScene>(l, bindLuaScene);
				return 1;
			});
		}

		void BindLuaScene::ClearScene()
		{
			mScene.Clear();
		}

		ECS::Entity BindLuaScene::CreateEntity()
		{
			return ECS::CreateEntity();
		}

		void BindLuaScene::RemoveEntity(ECS::Entity entity)
		{
			mScene.RemoveEntity(entity);
		}

		ECS::Entity BindLuaScene::DuplicateEntity(ECS::Entity entity)
		{
			return mScene.DuplicateEntity(entity);
		}

		LightComponent& BindLuaScene::CreateLight(ECS::Entity entity)
		{
			return mScene.GetOrCreateLightByEntity(entity);
		}

		TransformComponent & BindLuaScene::CreateTransform(ECS::Entity entity)
		{
			return mScene.GetOrCreateTransformByEntity(entity);
		}

		ECS::Entity BindLuaScene::LoadModel(const std::string& path)
		{
			return mScene.LoadModel(path);
		}

		TransformComponent* BindLuaScene::GetTransform(ECS::Entity entity)
		{
			TransformComponent* transform = mScene.mTransforms.GetComponent(entity);
			return transform;
		}

		int BindLuaScene::MouseRayCast(lua_State* l)
		{
			return LuaTools::ExceptionBoundary(l, [&] 
			{
				I32x2 mousePos = GlobalGetSubSystem<InputManager>().GetMousePos();
				Ray pickRay = Renderer::GetMainCameraMouseRay({ (U32)mousePos[0], (U32)mousePos[1] });
				Scene::PickResult ret = Scene::GetScene().PickObjects(pickRay);
				if (ret.entity == INVALID_ENTITY)
				{
					lua_pushnil(l);
					return 1;
				}

				BindLuaVector vector(XMLoad(ret.position));	
				LuaTools::Push<U32>(l, ret.entity);
				LuaTools::Push<BindLuaVector>(l, vector);
				return 2;
			});
		}

		void BindSceneModules(lua_State* l)
		{
			LuaBinder(l)
				.BeginClass<BindLuaScene>("Scene")
				.AddCFunction("GetMainScene", BindLuaScene::GetMainScene)
				.AddMethod("ClearScene", &BindLuaScene::ClearScene)
				.AddMethod("CreateEntity", &BindLuaScene::CreateEntity)
				.AddMethod("RemoveEntity", &BindLuaScene::RemoveEntity)
				.AddMethod("DuplicateEntity", &BindLuaScene::DuplicateEntity)		
				.AddMethod("CreateLight", &BindLuaScene::CreateLight)
				.AddMethod("CreateTransform", &BindLuaScene::CreateTransform)
				.AddMethod("LoadModel", &BindLuaScene::LoadModel)
				.AddMethod("GetTransform", &BindLuaScene::GetTransform)
				.AddCFunctionMethod("MouseRayCast", BindLuaScene::MouseRayCast)
				.EndClass();
		}
	}
}