#include "luaApi.h"
#include "matrixApi.h"
#include "vectorApi.h"
#include "sceneApi.h"
#include "uiApi.h"
#include "utilsApi.h"
#include "helper\logger.h"

namespace Cjing3D {
namespace LuaApi {

	void BindAllModules(lua_State * l)
	{
		BindUtilsModules(l);
		BindLuaMatrixApi(l);
		BindLuaVectorApi(l);

		BindSystemModules(l);
		BindRenderModules(l);
		BindSceneModules(l);
		BindLoggerModules(l);
		BindGUIModules(l);
	}

	int LuaLoggerInfo(lua_State* l)
	{
		return LuaTools::ExceptionBoundary(l, [&] {
			const std::string& msg = LuaTools::Get<std::string>(l, 1);
			Logger::Info("[Lua] " + msg);
			return 0;
		});
	}

	int LuaLoggerWarning(lua_State* l)
	{
		return LuaTools::ExceptionBoundary(l, [&] {
			const std::string& msg = LuaTools::Get<std::string>(l, 1);
			Logger::Warning("[Lua] " + msg);
			return 0;
		});
	}

	int LuaLoggerError(lua_State* l)
	{
		return LuaTools::ExceptionBoundary(l, [&] {
			const std::string& msg = LuaTools::Get<std::string>(l, 1);
			Logger::Error("[Lua] " + msg);
			return 0;
		});
	}

	void BindLoggerModules(lua_State * l)
	{
		LuaBinder(l)
			.BeginModule("Logger")
			.AddLuaCFunction("Info", LuaLoggerInfo)
			.AddLuaCFunction("Warning", LuaLoggerWarning)
			.AddLuaCFunction("Error", LuaLoggerError)
			.EndModule();
	}
}
}