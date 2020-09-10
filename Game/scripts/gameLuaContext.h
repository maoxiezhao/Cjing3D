#pragma once

#include "definitions\definitions.h"
#include "scripts\luaContext.h"

namespace CjingGame
{
	class GameLuaContext
	{
	public:
		GameLuaContext();
		~GameLuaContext();

		void Initialize();
		void FixedUpdate();
		void Uninitialize();

	private:
		void InitGameModulerAPI();
		void InitializeEnum();

	private:
		Cjing3D::LuaContext& mLuaContext;
	};
}