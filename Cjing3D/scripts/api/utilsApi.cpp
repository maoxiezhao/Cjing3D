#include "utilsApi.h"

namespace Cjing3D {
namespace LuaApi {
	void BindUtilsModules(lua_State* l)
	{
		LuaBinder(l)
			.BeginClass<BindLuaVariant>("Variant")
			.AddConstructor(_LUA_ARGS_())
			.AddMethod("GetFloat", &BindLuaVariant::GetFloat)
			.AddMethod("GetInt", &BindLuaVariant::GetInt)
			.AddMethod("GetString", &BindLuaVariant::GetString)
			.AddMethod("GetWidget", &BindLuaVariant::GetWidget)
			.EndClass();


		LuaBinder(l)
			.BeginClass<BindLuaVariantArray>("VariantArray")
			.AddConstructor(_LUA_ARGS_())
			.AddCFunctionMethod("GetVariantByIndex", BindLuaVariantArray::LuaGetVariantByIndex)
			.AddMethod("PushVariant", &BindLuaVariantArray::PushVariant)
			.EndClass();
	}

	BindLuaVariant BindLuaVariantArray::GetVariantByIndex(U32 index)
	{
		if (index < 0 || index >= mValues.size()) {
			return BindLuaVariant();
		}
		return mValues[index];
	}

	void BindLuaVariantArray::PushVariant(BindLuaVariant variant)
	{
		mValues.push_back(variant);
	}

	void BindLuaVariantArray::EmplaceBack(Variant value)
	{
		mValues.emplace_back(value);
	}

	U32 BindLuaVariantArray::GetSize()
	{
		return mValues.size();
	}

	int BindLuaVariantArray::LuaGetVariantByIndex(lua_State* l)
	{
		return LuaTools::ExceptionBoundary(l, [&] {
			BindLuaVariantArray& variantArray = LuaTools::Get<BindLuaVariantArray&>(l, 1);
			U32 index = LuaTools::Get<U32>(l, 2);
			if (index < 0 || index >= variantArray.GetSize()) {
				return 0;
			}

			BindLuaVariant variant = variantArray.GetVariantByIndex(index);
			LuaTools::Push< BindLuaVariant>(l, variant);
			return 1;
		});
	}
}
}
