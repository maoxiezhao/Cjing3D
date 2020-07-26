#include "game\levelEditor.h"
#include "gui\guiEditor\guiEditorInclude.h"

using namespace Cjing3D;

namespace CjingGame
{
	namespace {
		void ShowEditorWindow(F32 deltaTime, LevelEditor& levelEditor)
		{
			ImGuiWindowFlags window_flags = 0;
			window_flags |= ImGuiWindowFlags_NoTitleBar;
			window_flags |= ImGuiWindowFlags_NoScrollbar;
			window_flags |= ImGuiWindowFlags_MenuBar;
			window_flags |= ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoResize;

			ImGui::SetNextWindowPos(ImVec2(500, 20), ImGuiCond_Once);
			ImGui::SetNextWindowSize(ImVec2(900, 50), ImGuiCond_Once);
			ImGui::Begin("MapEditor", nullptr, window_flags);

			// menu bar
			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("File"))
				{
					if (ImGui::MenuItem("New Map")) { Scene::GetScene().Clear(); }
					if (ImGui::MenuItem("Load Map")) { }
					if (ImGui::MenuItem("Save Map")) { }
					ImGui::EndMenu();
				}
			}
			ImGui::EndMenuBar();

			// mode radio button
			static EditorMode mode = levelEditor.GetEditorMode();
			if (ImGui::RadioButton("Ground", mode == EditorMode_Ground)) { mode = EditorMode_Ground; } ImGui::SameLine();
			if (ImGui::RadioButton("Wall",   mode == EditorMode_Wall)) { mode = EditorMode_Wall; }ImGui::SameLine(200.0f);
			levelEditor.SetEditorMode(mode);

			bool isGridVisible = levelEditor.IsDebugGridVisible();
			if (ImGui::Checkbox("Grid", &isGridVisible))
			{
				levelEditor.SetDebugGridVisible(isGridVisible);
			}

			ImGui::End();
		}
	}

	void LevelEditor::InitializeEditorGUI(IMGUIStage& imguiStage)
	{
		imguiStage.RegisterCustomWindow([this](F32 deltaTime) {ShowEditorWindow(deltaTime, *this); });
	}
}