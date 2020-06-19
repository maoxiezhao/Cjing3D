#pragma once

#include "scripts\luaBinder.h"
#include "scripts\luaTypeMapping.h"
#include "helper\variant.h"
#include "gui\guiWidgets\widgets.h"

#include <vector>

namespace Cjing3D{
namespace LuaApi
{
	void BindUtilsModules(lua_State* l);

	class BindLuaVariant
	{
	public:
		BindLuaVariant() {}
		BindLuaVariant(Variant& variant) : mValue(variant) {}
		~BindLuaVariant() {}

		Variant& GetVariant() { return mValue; }
		void SetVariant(Variant variant) { mValue = variant; }
		void SetWidget(Gui::WidgetPtr widget) { mWidget = widget; }

		F32 GetFloat() { return mValue.GetValue<F32>(); }
		I32 GetInt() { return mValue.GetValue<I32>(); }
		std::string GetString() { return mValue.GetValue<std::string>(); }
		Gui::WidgetPtr GetWidget() { return mWidget; }

		Variant mValue;
		Gui::WidgetPtr mWidget = nullptr;
	};

	class BindLuaVariantArray
	{
	public:
		BindLuaVariantArray() {}
		~BindLuaVariantArray() {}

		BindLuaVariant GetVariantByIndex(U32 index);
		void PushVariant(BindLuaVariant variant);
		void EmplaceBack(Variant value);

		U32 GetSize();

		static int LuaGetVariantByIndex(lua_State* l);

	private:
		std::vector<BindLuaVariant> mValues;
	};
}
}