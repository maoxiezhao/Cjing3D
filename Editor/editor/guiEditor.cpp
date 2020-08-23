#include "guiEditor.h"
#include "gui\guiStage.h"
#include "gui\guiEditor\guiEditorInclude.h"
#include "editor\widget\guiEditorMenuBar.h"
#include "editor\widget\guiEditorToolbar.h"
#include "editor\widget\guiEditorEntityList.h"
#include "editor\widget\guiEditorProperties.h"

namespace Cjing3D {

	namespace {
		EditorWidget* mWidgetMenu = nullptr;
		EditorWidget* mWidgetToolbar = nullptr;
	}

	EditorStage::EditorStage(GUIRenderer& renderer) :
		IMGUIStage(renderer)
	{
		mConfigFlag =
			ImGuiConfigFlags_NavEnableKeyboard |
			ImGuiConfigFlags_DockingEnable; /* |
		    ImGuiConfigFlags_ViewportsEnable;*/
	}

	EditorStage::~EditorStage()
	{
	}

	void EditorStage::InitializeImpl()
	{
		RegisterCustomWidget(CJING_MAKE_SHARED<EditorWidgetMenu>(*this));
		RegisterCustomWidget(CJING_MAKE_SHARED<EditorWidgetToolbar>(*this));
		RegisterCustomWidget(CJING_MAKE_SHARED<EditorWidgetEntityList>(*this));
		RegisterCustomWidget(CJING_MAKE_SHARED<EditorWidgetProperties>(*this));

		mWidgetMenu = GetCustomWidget<EditorWidgetMenu>();
		mWidgetToolbar = GetCustomWidget<EditorWidgetToolbar>();
	}

	void EditorStage::DockingBegin()
	{
		// full screen without window
		ImGuiWindowFlags windowFlags =
			ImGuiWindowFlags_MenuBar |
			ImGuiWindowFlags_NoDocking |
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoBringToFrontOnFocus |
			ImGuiWindowFlags_NoNavFocus;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::SetNextWindowBgAlpha(0.0f);

		F32 dockingOffsetY = 0.0f;
		dockingOffsetY += mWidgetToolbar->GetHeight();
		dockingOffsetY += mWidgetMenu->GetHeight();

		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x,   viewport->Pos.y  + dockingOffsetY));
		ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, viewport->Size.y - dockingOffsetY));
		ImGui::SetNextWindowViewport(viewport->ID);

		static const char* dockingName = "CjingDock";
		mIsDockingBegin = ImGui::Begin(dockingName, nullptr, windowFlags);
		ImGui::PopStyleVar(3);

		if (mIsDockingBegin)
		{
			ImGuiID mainWindow = ImGui::GetID(dockingName);
			if (!ImGui::DockBuilderGetNode(mainWindow))
			{
				// reset
				ImGui::DockBuilderRemoveNode(mainWindow);
				ImGui::DockBuilderAddNode(mainWindow, ImGuiDockNodeFlags_None);
				ImGui::DockBuilderSetNodeSize(mainWindow, viewport->Size);

				ImGuiID dockMainID = mainWindow;
				// split node
				ImGuiID dockLeft  = ImGui::DockBuilderSplitNode(mainWindow, ImGuiDir_Left, 0.2f, nullptr, &dockMainID);
				ImGuiID dockRight = ImGui::DockBuilderSplitNode(mainWindow, ImGuiDir_Left, 0.6f, nullptr, &dockMainID);

				// build window
				ImGui::DockBuilderDockWindow("EntityList", dockLeft);
				ImGui::DockBuilderDockWindow("Properties", dockRight);
				ImGui::DockBuilderFinish(mainWindow);
			}

			ImGui::DockSpace(mainWindow, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
		}
	}

	void EditorStage::DockingEnd()
	{
		if (mIsDockingBegin) {
			ImGui::End();
		}
	}
}

