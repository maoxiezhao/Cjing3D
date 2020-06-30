#include "guiEditorGUI.h"
#include "guiEditor.h"
#include "gui\guiEditor\guiEditorInclude.h"

#include "gui\guiStage.h"
#include "gui\guiWidgets\widgetHierarchy.h"
#include "gui\guiCore\guiDistributor.h"
#include "gui\guiWidgets\widgets.h"
#include "input\InputSystem.h"

namespace Cjing3D {
namespace Editor {

	void ShowGUIMenu()
	{
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Add Widget")) {  }
				ImGui::EndMenu();
			}
			
			ImGui::EndMenuBar();
		}
	}

	bool bShowGUIWindow = false;
	void ShowGUIWindowImpl(F32 deltaTime)
	{
		if (bShowGUIWindow == false) return;

		ImGui::SetNextWindowPos(ImVec2(500, 100), ImGuiCond_Once);
		ImGui::SetNextWindowSize(ImVec2(350, 200), ImGuiCond_Once);
		ImGui::Begin("GUI Editor", &bShowGUIWindow);

		ShowGUIMenu();

		GUIStage& guiStage = GlobalGetSubSystem<GUIStage>();
		InputManager& inputManager = GlobalGetSubSystem<InputManager>();
		Gui::WidgetHierarchy& widgetHierachy = guiStage.GetWidgetHierarchy();

		ImGui::Text("GUI Info");
		ImGui::Separator();

		I32x2 mousePos = inputManager.GetMousePos();
		ImGui::Text("Current mouse pos: %d %d", mousePos[0], mousePos[1]);

		// current mouse focuse widget
		Gui::WidgetPtr mouseWidget = widgetHierachy.GetCurrentMouseFocusdWidget();
		if (mouseWidget != nullptr)
		{
			std::string text = "Current mouse on widget:" + mouseWidget->GetName().GetString();
			ImGui::Text(text.c_str());
		}
		else
		{
			ImGui::Text("Current mouse on widget: NULL");
		}

		// current mouse drage widget
		Gui::WidgetPtr dragWidget = widgetHierachy.GetCurrentDragWidget();
		if (dragWidget != nullptr)
		{
			std::string text = "Current mouse drag widget:" + dragWidget->GetName().GetString();
			ImGui::Text(text.c_str());
		}
		else
		{
			ImGui::Text("Current mouse drag widget: NULL");
		}

		// current mouse button focus widget
		Gui::WidgetPtr focusedWidget = widgetHierachy.GetCurrentFocusdWidget();
		if (focusedWidget != nullptr)
		{
			std::string text = "Current focused widget:" + focusedWidget->GetName().GetString();
			ImGui::Text(text.c_str());
		}
		else
		{
			ImGui::Text("Current focused widget: NULL");
		}


		ImGui::End();
	}

	void InitializeEditorGUI(IMGUIStage& imguiStage)
	{
		imguiStage.RegisterCustomWindow(ShowGUIWindowImpl);
	}

	void ShowGUIWindow()
	{
		bShowGUIWindow = true;
	}
}
}