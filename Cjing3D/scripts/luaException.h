#pragma once

#include "scripts\luaCommon.h"

#include <exception>
#include <string>

namespace Cjing3D {

	class LuaException : public std::exception
	{
	public:
		LuaException(lua_State*l, const std::string& errmsg) : l(l), mErrMsg(errmsg) {};
		lua_State* GetLuaState()const { return l; }
		virtual const char* what()const throw() override { return mErrMsg.c_str(); }

		static void Error(lua_State * l, const std::string & message);
		static void ArgError(lua_State * l, int index, const std::string & message);
	private:
		std::string mErrMsg;
		lua_State* l;
	};
}