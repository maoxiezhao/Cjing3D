#include "luaApi.h"
#include "vectorApi.h"
#include "matrixApi.h"

namespace Cjing3D {
namespace LuaApi {

	void BindLuaVectorApi(lua_State* l)
	{
		LuaBinder(l)
			.BeginClass<BindLuaVector>("Vector")
			.AddCFunctionConstructor(BindLuaVector::Create)
			.AddMethod("SetX", &BindLuaVector::SetX)
			.AddMethod("SetY", &BindLuaVector::SetY)
			.AddMethod("SetZ", &BindLuaVector::SetZ)
			.AddMethod("SetW", &BindLuaVector::SetW)
			.AddMethod("SetXYZ", &BindLuaVector::SetXYZ)
			.AddMethod("GetX", &BindLuaVector::GetX)
			.AddMethod("GetY", &BindLuaVector::GetY)
			.AddMethod("GetZ", &BindLuaVector::GetZ)
			.AddMethod("GetW", &BindLuaVector::GetW)
			.AddMethod("Normalize", &BindLuaVector::Normalize)
			.AddMethod("Length", &BindLuaVector::Length)
            .AddFunction("Multiply", &BindLuaVector::Multiply)
			.AddFunction("MultiplyWithNumber", &BindLuaVector::MultiplyWithNumber)
			.AddFunction("Transform", &BindLuaVector::Transform)
			.AddFunction("TransformNormal", &BindLuaVector::TransformNormal)
			.EndClass();
		
	}

	BindLuaVector::~BindLuaVector()
	{
	}

	int BindLuaVector::Create(lua_State * l)
	{
		return LuaTools::ExceptionBoundary(l, [&] {

			F32 x = 0.0f, y = 0.0f, z = 0.0f, w = 0.0f;

			int argCount = LuaTools::GetArgCount(l);
			if (argCount > 1) {

				x = LuaTools::Get<F32>(l, 2);
				if (argCount > 2) {

					y = LuaTools::Get<F32>(l, 3);
					if (argCount > 3) {

						z = LuaTools::Get<F32>(l, 4);
						if (argCount > 4) {
							w = LuaTools::Get<F32>(l, 5);
						}
					}
				}
			}

			BindLuaVector vector(XMVectorSet(x, y, z, w));
			LuaTools::Push<BindLuaVector>(l, vector);
			return 1;
		});
	}

	BindLuaVector BindLuaVector::Multiply(const BindLuaVector & v1, const BindLuaVector & v2)
	{
		return BindLuaVector(XMVectorMultiply(v1.mValue, v2.mValue));
	}

	BindLuaVector BindLuaVector::MultiplyWithNumber(const BindLuaVector & v, const F32 & number)
	{
		return BindLuaVector(v.mValue * number);
	}

	BindLuaVector BindLuaVector::Transform(const BindLuaVector & vec, const BindLuaMatrix & mat)
	{
		XMVECTOR newVector = XMVector4Transform(vec.mValue, mat.mValue);
		return BindLuaVector(newVector);
	}

	BindLuaVector BindLuaVector::TransformNormal(const BindLuaVector & vec, const BindLuaMatrix & mat)
	{
		XMVECTOR newVector = XMVector3TransformNormal(vec.mValue, mat.mValue);
		return BindLuaVector(newVector);
	}

}
}