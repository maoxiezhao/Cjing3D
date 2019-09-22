#include "luaException.h"

#include<iostream>
#include<sstream>


namespace Cjing3D
{
	void LuaException::Error(lua_State * l, const std::string & message)
	{
		std::string msg(message);
		luaL_traceback(l, l, NULL, 1);
		msg += lua_tostring(l, -1);

		std::cout << msg << std::endl;
		throw LuaException(l, msg);
	}

	void LuaException::ArgError(lua_State * l, int index, const std::string & message)
	{
		std::ostringstream oss;
		lua_Debug info;
		if (!lua_getstack(l, 0, &info))		// 获取当前运行函数的活动记录
		{
			oss << "Bad argument #" << index << "(" << message << ")";
			Error(l, oss.str());
		}

		lua_getinfo(l, "n", &info);			// 填充name域，即当前函数的名字域
		if (std::string(info.namewhat) == "method")
		{
			// ?????
			oss << "Calling:" << info.name << " failed. (" << message << ")";
			Error(l, oss.str());
		}

		if (info.name == nullptr)
			info.name = "?";

		oss << "Bad argument #" << index << " to " << info.name << "(" << message << ")";
		Error(l, oss.str());
	}
}