#include "luaContext.h"
#include "helper\debug.h"
#include "helper\fileSystem.h"

namespace Cjing3D {

LuaRef LuaContext::mSystemExports;

LuaContext::LuaContext(SystemContext& systemContext) :
	SubSystem(systemContext)
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

	LuaApi::BindAllModules(mLuaState);

	InitializeEnv(mLuaState);

	DoFileIfExists(mLuaState, "Scripts/main");
	OnMainInitialize();
}

void LuaContext::InitializeEnv(lua_State * l)
{
	if (!DoLuaString(l, globalLuaString))
	{
		Debug::Error("Failed to load lua initialzed scripts.");
		return;
	}

	lua_getglobal(l, "SystemExports");
	Debug::CheckAssertion(!lua_isnil(l, 1), "Lua env initialized failed.");
	mSystemExports = LuaRef::CreateRef(l);

	AutoLuaBindFunctions::GetInstance().DoAutoBindFunctions(mLuaState);
}

void LuaContext::Update(F32 deltaTime)
{
	OnMainUpdate();
}

void LuaContext::Uninitialize()
{
	OnMainUninitialize();

	mSystemExports.Clear();

	lua_close(mLuaState);
	mLuaState = nullptr;
}

bool LuaContext::DoLuaString(lua_State * l, const std::string & luaString)
{
	if (luaL_loadstring(l, luaString.c_str()) == 0) {
		return LuaTools::CallFunction(l, 0, 0, "Load Lua String.");
	}
	return false;
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

void LuaContext::OnMainInitialize()
{
	DoLuaSystemFunctionWithIndex(CLIENT_LUA_MAIN_START);
}

void LuaContext::OnMainUpdate()
{
	DoLuaSystemFunctionWithIndex(CLIENT_LUA_MAIN_UPDATE);
}

void LuaContext::OnMainUninitialize()
{
	DoLuaSystemFunctionWithIndex(CLIENT_LUA_MAIN_STOP);
}

bool LuaContext::DoLuaSystemFunctionWithIndex(SystemFunctionIndex index)
{
	mSystemExports.Push();

	lua_rawgeti(mLuaState, -1, static_cast<int>(index));
	if (!LuaTools::CallFunction(mLuaState, 0, 0, ""))
	{
		lua_pop(mLuaState, 1);
		return false;
	}

	lua_pop(mLuaState, 1);
	return true;
}

int LuaContext::api_panic(lua_State*l)
{
	return 0;
}

}
