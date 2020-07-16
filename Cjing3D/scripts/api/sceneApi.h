#pragma once

#include "scripts\luaBinder.h"
#include "scripts\luaTypeMapping.h"
#include "system\sceneSystem.h"

namespace Cjing3D
{
	namespace LuaApi
	{
		void BindSceneModules(lua_State* l);

		// bind lua vector, manual binding
		class BindLuaScene
		{
		public:
			BindLuaScene(Scene& scene) :mScene(scene) {};

			Scene& GetScene() { return mScene; }

			static int GetMainScene(lua_State* l);
			
			ECS::Entity CreateEntity();
			LightComponent& CreateLight(ECS::Entity entity);
			TransformComponent& CreateTransform(ECS::Entity entity);
			ECS::Entity LoadModel(const std::string& path);

			TransformComponent* GetTransform(ECS::Entity entity);

			static int MouseRayCast(lua_State* l);

		private:
			Scene & mScene;
		};
	}
}