#pragma once

#include "core\subSystem.hpp"

#include "scripts\luaRef.h"
#include "scripts\luaTools.h"
#include "scripts\api\luaApi.h"
#include "helper\enumInfo.h"

#include <string>

namespace Cjing3D
{

enum SystemFunctionIndex {
	CLIENT_LUA_MAIN_START = 1,
	CLIENT_LUA_MAIN_UPDATE,
	CLIENT_LUA_MAIN_STOP,
};

class LuaContext : public SubSystem
{
public:
	LuaContext(SystemContext& systemContext);
	~LuaContext();

	void Initialize();
	void Uninitialize();
	void Update(F32 deltaTime);

	using FunctionExportToLua = int(lua_State* l);
	static FunctionExportToLua
		api_panic;

	static bool DoLuaString(lua_State*l, const std::string& luaString);
	static bool DoFileIfExists(lua_State*l, const std::string& name);
	static bool LoadFile(lua_State*l, const std::string& name);

	lua_State* GetLuaState() { return mLuaState; }

	// main function 
	void OnMainInitialize();
	void OnMainUpdate();
	void OnMainUninitialize();

	// system lua ref
	static LuaRef mSystemExports;
	static LuaRef mSystemEnumRef;

	bool DoLuaSystemFunctionWithIndex(SystemFunctionIndex index);

private:
	void InitializeEnv(lua_State* l);
	void InitializeEnum(lua_State* l);

	lua_State* mLuaState = nullptr;
};

ENUM_TRAITS_REGISTER_ENUM_HEADER(SystemFunctionIndex)

#include "luaContext.inl"

}