#pragma once

#include "core\subSystem.hpp"

#include "scripts\luaRef.h"
#include "scripts\luaTools.h"
#include "scripts\api\luaApi.h"

#include <string>

namespace Cjing3D
{
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

	bool DoFileIfExists(const std::string& name);
	static bool DoFileIfExists(lua_State*l, const std::string& name);
	static bool LoadFile(lua_State*l, const std::string& name);

	lua_State* GetLuaState() { return mLuaState; }

private:

	lua_State* mLuaState = nullptr;
};
}