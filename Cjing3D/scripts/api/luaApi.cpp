#include "luaApi.h"

namespace Cjing3D {
namespace LuaApi {

	void BindAllModules(lua_State * l)
	{
		BindMainComponent(l);
		BindInputManager(l);
		BindSceneSystem(l);
	}
}
}