#include "gameLuaContext.h"

using namespace Cjing3D;

namespace CjingGame
{
	GameLuaContext::GameLuaContext() :
		mLuaContext(*GetGlobalContext().gLuaContext)
	{
	}

	GameLuaContext::~GameLuaContext()
	{
	}

	void GameLuaContext::Initialize()
	{
		InitGameModulerAPI();
		InitializeEnum();
	}

	void GameLuaContext::FixedUpdate()
	{
	}

	void GameLuaContext::Uninitialize()
	{
	}

	void GameLuaContext::InitGameModulerAPI()
	{
	}

	void GameLuaContext::InitializeEnum()
	{
	}
}