#include "luaApi.h"
#include "vectorApi.h"
#include "matrixApi.h"

namespace Cjing3D {
namespace LuaApi {

	void BindLuaMatrixApi(lua_State * l)
	{
		LuaBinder(l)
			.BeginClass<BindLuaMatrix>("Matrix")
			.AddCFunctionConstructor(BindLuaMatrix::Create)
			.AddMethod("Translate", &BindLuaMatrix::Translate)
			.AddMethod("Rotation", &BindLuaMatrix::Rotation)
			.AddMethod("RotationQuaternion", &BindLuaMatrix::RotationQuaternion)
			.EndClass();
	}

	int BindLuaMatrix::Create(lua_State * l)
	{
		return LuaTools::ExceptionBoundary(l, [&] {
			XMMATRIX matrix = XMMatrixIdentity();

			int argCount = LuaTools::GetArgCount(l);
			for (int i = 0; i < 4; ++i)
			{
				float x = 0.f, y = 0.f, z = 0.f, w = 0.f;
				if (argCount > i * 4 + 1)
				{
					x = LuaTools::Get<F32>(l, i * 4 + 2);
					if (argCount > i * 4 + 2)
					{
						y = LuaTools::Get<F32>(l, i * 4 + 3);
						if (argCount > i * 4 + 3)
						{
							z = LuaTools::Get<F32>(l, i * 4 + 4);
							if (argCount > i * 4 + 4)
							{
								w = LuaTools::Get<F32>(l, i * 4 + 5);
							}
						}
					}
				}
				matrix.r[i] = XMVectorSet(x, y, z, w);
			}

			BindLuaMatrix result(matrix);
			LuaTools::Push<BindLuaMatrix>(l, result);
			return 1;
		});
	}

	void BindLuaMatrix::Translate(const BindLuaVector & vec)
	{
		mValue = XMMatrixTranslationFromVector(vec.mValue);
	}

	void BindLuaMatrix::Rotation(const BindLuaVector & vec)
	{
		mValue = XMMatrixRotationRollPitchYawFromVector(vec.mValue);
	}

	void BindLuaMatrix::RotationQuaternion(const BindLuaVector & vec)
	{
		mValue = XMMatrixRotationQuaternion(vec.mValue);
	}

}
}