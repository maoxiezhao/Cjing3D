#include "luaContext.h"
#include "helper\debug.h"
#include "helper\fileSystem.h"

namespace Cjing3D {

LuaContext::LuaContext()
{
}

LuaContext::~LuaContext()
{
}

void LuaContext::Initialize()
{
	mLuaState = luaL_newstate();
	lua_atpanic(mLuaState, api_panic);
	luaL_openlibs(mLuaState);
}

void LuaContext::Update()
{
}

void LuaContext::Uninitialize()
{
	lua_close(mLuaState);
	mLuaState = nullptr;
}

bool LuaContext::DoFileIfExists(lua_State* l, const std::string& name)
{
	if (LoadFile(l, name))
	{
		if (lua_pcall(l, 0, 0, 0) != 0)
		{
			std::string errMsg = std::string("In ") + name + ": " + lua_tostring(l, -1) + "\n";
			luaL_traceback(l, l, NULL, 1);
			errMsg += lua_tostring(l, -1);
			Debug::Error(errMsg);
			lua_pop(l, 2);
			return false;
		}
		return true;
	}
	return false;
}

bool LuaContext::LoadFile(lua_State * l, const std::string & name)
{
	std::string fileName(name);
	// 如果不存在，在末尾加上Lua后再尝试
	//if (!FileData::IsFileExists(fileName))
	if ((fileName.find(".lua") == std::string::npos) ||
		!FileData::IsFileExists(fileName))
	{
		fileName += ".lua";

		// 如果依旧不存在则返回
		if (!FileData::IsFileExists(fileName))
			return false;
	}
	const std::string buffer = FileData::ReadFile(fileName);
	int result = luaL_loadbuffer(l, buffer.data(), buffer.size(), fileName.c_str());
	if (result != 0)
	{
		const std::string& errMsg = lua_tostring(l, -1);
		Debug::Warning(errMsg);
		lua_pop(l, 1);
		return false;
	}
	return true;
}

int LuaContext::api_panic(lua_State*l)
{
	return 0;
}

}
