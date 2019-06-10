#pragma once

#include "scripts\luaRef.h"
#include "scripts\luaTools.h"
#include "core\subSystem.hpp"

#include <string>

namespace Cjing3D
{
class LuaContext : public SubSystem
{
public:
	LuaContext(SystemContext& systemContext);
	~LuaContext();

	virtual void Initialize();
	virtual void Uninitialize();
	void Update();

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