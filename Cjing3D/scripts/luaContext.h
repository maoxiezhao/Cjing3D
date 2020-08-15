#pragma once

#include "core\subSystem.hpp"

#include "scripts\binder\luaRef.h"
#include "scripts\binder\luaTools.h"
#include "scripts\api\luaApi.h"
#include "helper\enumInfo.h"

#include <string>

namespace Cjing3D
{

enum SystemFunctionIndex {
	CLIENT_LUA_MAIN_INITIALIZE = 1,
	CLIENT_LUA_MAIN_START,
	CLIENT_LUA_MAIN_UPDATE,
	CLIENT_LUA_MAIN_FIXED_UPDATE,
	CLIENT_LUA_MAIN_STOP,
	CLIENT_LUA_MAIN_CHANGE_SCENE,
};

class LuaContext : public SubSystem
{
public:
	LuaContext(GlobalContext& globalContext);
	~LuaContext();

	void Initialize();
	void Uninitialize()override;
	void FixedUpdate();
	void Update(F32 deltaTime);
	void GC();

	using FunctionExportToLua = int(lua_State* l);
	static FunctionExportToLua
		api_panic;

	static bool DoLuaString(lua_State*l, const std::string& luaString, int arguments = 0, int results = 0);
	static bool DoFileIfExists(lua_State*l, const std::string& name);
	static bool LoadFile(lua_State*l, const std::string& name);

	lua_State* GetLuaState() { return mLuaState; }

	// system lua ref
	static LuaRef mSystemExports;
	static LuaRef mSystemEnumRef;

	bool DoLuaSystemFunctionWithIndex(SystemFunctionIndex index);

	void StartMainScript();
	void StopMainScript();
	void ChangeLuaScene(const std::string& name);

private:
	void InitializeEnv(lua_State* l);
	void InitializeEnum(lua_State* l);

	// main function 
	void OnMainInitialize();
	void OnMainStart();
	void OnMainUpdate();
	void OnMainFixedUpdate();
	void OnMainUninitialize();

	lua_State* mLuaState = nullptr;
};

ENUM_TRAITS_REGISTER_ENUM_HEADER(SystemFunctionIndex)

#include "luaContext.inl"

}