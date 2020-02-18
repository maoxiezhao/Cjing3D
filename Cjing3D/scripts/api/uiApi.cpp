#include "uiApi.h"

namespace Cjing3D {
	namespace LuaApi {

		int LoadWidgetFromXML(lua_State* l)
		{
			return LuaTools::ExceptionBoundary(l, [&] {
				StringID name = LuaTools::Get<StringID>(l, 1);
				std::string path = LuaTools::Get<std::string>(l, 2);
				LuaRef handle = LuaTools::Get<LuaRef>(l, 3);

				GUIStage& guiStage = SystemContext::GetSystemContext().GetSubSystem<GUIStage>();
				WidgetPtr widget = guiStage.LoadWidgetFromXML(name, path, handle);

				LuaTools::Push<WidgetPtr>(l, widget);
				return 1;
			});
		}

		void BindGUIModules(lua_State* l)
		{
			LuaBinder(l)
				.BeginModule("Gui")
				.AddLuaCFunction("LoadWidgetFromXML", LoadWidgetFromXML)
				.EndModule();
		}
	}
}