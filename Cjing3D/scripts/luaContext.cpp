#include "luaContext.h"
#include "luaBinder.h"
#include "helper\debug.h"
#include "helper\fileSystem.h"
#include "helper\profiler.h"
#include "input\InputSystem.h"

namespace Cjing3D {

LuaRef LuaContext::mSystemExports;

ENUM_TRAITS_REGISTER_ENUM_BEGIN(SystemFunctionIndex)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(CLIENT_LUA_MAIN_INITIALIZE)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(CLIENT_LUA_MAIN_START)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(CLIENT_LUA_MAIN_UPDATE)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(CLIENT_LUA_MAIN_STOP)
ENUM_TRAITS_REGISTER_ENUM_END(SystemFunctionIndex)

LuaContext::LuaContext(SystemContext & systemContext) :
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

	InitializeEnum(l);

	AutoLuaBindFunctions::REGISTER_AUTO_BINDING_FUNC();
	AutoLuaBindFunctions::GetInstance().DoAutoBindFunctions(mLuaState);
}

void LuaContext::InitializeEnum(lua_State * l)
{
	// bind systme enum
	auto systemEnumBinder = LuaBinder(l).BeginModule("SystemFunctionIndex");
	systemEnumBinder.AddEnum(EnumToString(CLIENT_LUA_MAIN_INITIALIZE), CLIENT_LUA_MAIN_INITIALIZE);
	systemEnumBinder.AddEnum(EnumToString(CLIENT_LUA_MAIN_START), CLIENT_LUA_MAIN_START);
	systemEnumBinder.AddEnum(EnumToString(CLIENT_LUA_MAIN_UPDATE), CLIENT_LUA_MAIN_UPDATE);
	systemEnumBinder.AddEnum(EnumToString(CLIENT_LUA_MAIN_STOP), CLIENT_LUA_MAIN_STOP);

	// bind input enum
	auto keyEnumBinder = LuaBinder(l).BeginModule("KeyCode");
	for (int index = 0; index < (int)(KeyCode::key_count); index++)
	{
		KeyCode keyCode = static_cast<KeyCode>(index);
		keyEnumBinder.AddEnum(EnumToString(keyCode), keyCode);
	}
}

void LuaContext::Update(F32 deltaTime)
{
	auto& profiler = Profiler::GetInstance();
	profiler.BeginBlock("LuaUpdate");
	OnMainUpdate();
	profiler.EndBlock();

	profiler.BeginBlock("LuaGC");
	GC();
	profiler.EndBlock();
}

void LuaContext::GC()
{
	// TEMP:暂时在每一帧结束后执行一次step gc
	lua_gc(mLuaState, LUA_GCSTEP, 200);
}

void LuaContext::Uninitialize()
{
	mSystemExports.Clear();

	lua_close(mLuaState);
	mLuaState = nullptr;
}

void LuaContext::FixedUpdate()
{
}

bool LuaContext::DoLuaString(lua_State * l, const std::string & luaString, int arguments, int results)
{
	if (luaL_loadstring(l, luaString.c_str()) == 0) {
		if (arguments > 0) {
			// stack: arg1 arg2 func
			lua_insert(l, -(arguments + 1));
			// func stack: arg1 arg2
		}

		return LuaTools::CallFunction(l, arguments, results, "Load Lua String.");
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
	DoLuaSystemFunctionWithIndex(CLIENT_LUA_MAIN_INITIALIZE);
}

void LuaContext::OnMainStart()
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
