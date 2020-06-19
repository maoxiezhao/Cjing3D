#pragma once

#include "gui\guiInclude.h"
#include "gui\guiRenderer.h"
#include "gui\guiWidgets\widgets.h"
#include "gui\guiWidgets\widgetHierarchy.h"
#include "gui\guiCore\guiEvents.h"

#include <queue>

namespace Cjing3D
{
	class IMGUIStage;

	namespace Gui {
		class WidgetManager;
	}

	LUA_BINDER_REGISTER_CLASS
	class GUIStage : public SubSystem
	{
	public:
		friend class GUIRenderer;

		LUA_BINDER_REGISTER_CLASS_CONSTRUCTOR
		GUIStage(SystemContext& systemContext);
		~GUIStage();

		void Initialize();
		void Uninitialize();
		void Update(F32 deltaTime);
		void FixedUpdate();

		GUIRenderer& GetGUIRenderer() { return *mRenderer; }
		const GUIRenderer& GetGUIRenderer()const { return *mRenderer; }
		Gui::WidgetManager& GetWidgetManager() { return *mWidgetManager; }

		Gui::WidgetPtr LoadWidgetFromXML(const StringID& name, const std::string& filePath, LuaRef scriptHandler = LuaRef::NULL_REF);
		Gui::WidgetPtr LoadWidgetFromXML(Gui::WidgetPtr parent, const StringID& name, const std::string& filePath, LuaRef scriptHandler = LuaRef::NULL_REF);

		void AddRegisterKeyBoardKey(KeyCode key);
		bool IsGUIVisible()const { return mGUIVisible; }
		void SetGUIVisible(bool visible) { mGUIVisible = visible; }

		// imgui
		void SetImGUIStageVisible(bool visible);
		bool IsImGUIStageVisible()const;
		IMGUIStage& GetImGUIStage();

	private:
		void LoadRegisteredKeys();
		void NotifyInput();
		void HandleInputEvents();
		void RenderImpl();

	private:
		// GUI Renderer
		// 在renderPath2D的render阶段会调用GUIRenderer的render
		std::unique_ptr<GUIRenderer> mRenderer = nullptr;
		std::unique_ptr<Gui::WidgetManager> mWidgetManager = nullptr;
		std::unique_ptr<Gui::WidgetHierarchy> mWidgetHierarchy = nullptr;
		std::vector<KeyCode> mRegisteredKeyBoardKeys;
		std::vector<KeyCode> mRegisteredMouseKeys;
		std::queue<Gui::GUIInputEvent> mInputEventQueue;

		I32x2 mPrevMousePos;
		bool mGUIVisible = true;
	};
}