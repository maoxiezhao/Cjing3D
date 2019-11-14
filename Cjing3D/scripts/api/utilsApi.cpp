#include "luaApi.h"
#include "utilsApi.h"

namespace Cjing3D {
namespace LuaApi {

	void BindUtilsModules(lua_State* l)
	{
		LuaBinder(l)
			.BeginClass<BindLuaVector>("Vector")
			.AddCFunctionConstructor(BindLuaVector::Create)
			.AddMethod("SetX", &BindLuaVector::SetX)
			.AddMethod("SetY", &BindLuaVector::SetY)
			.AddMethod("SetZ", &BindLuaVector::SetZ)
			.AddMethod("SetW", &BindLuaVector::SetW)
			.AddMethod("GetX", &BindLuaVector::GetX)
			.AddMethod("GetY", &BindLuaVector::GetY)
			.AddMethod("GetZ", &BindLuaVector::GetZ)
			.AddMethod("GetW", &BindLuaVector::GetW)
			.EndClass();
		
	}

	int BindLuaVector::Create(lua_State * l)
	{
		F32 x = 0.0f, y = 0.0f, z = 0.0f, w = 0.0f;

		int argCount = LuaTools::GetArgCount(l);
		if (argCount > 1){

			x = LuaTools::Get<F32>(l, 2);
			if (argCount > 2){

				y = LuaTools::Get<F32>(l, 3);
				if (argCount > 3){

					z = LuaTools::Get<F32>(l, 4);
					if (argCount > 4){
						w = LuaTools::Get<F32>(l, 5);
					}
				}
			}
		}


		BindLuaVector vector(XMVectorSet(x, y, z, w));
		LuaTools::Push<BindLuaVector>(l, vector);
		return 1;
	}
}
}