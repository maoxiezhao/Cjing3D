#include "game\levelEditor.h"
#include "gui\guiEditor\guiEditorInclude.h"
#include "platform\gameWindow.h"

using namespace Cjing3D;

namespace CjingGame
{
	namespace {
		void ShowGroundEditorWindow(LevelEditor& levelEditor)
		{
			ImGuiWindowFlags window_flags = 
				ImGuiWindowFlags_NoScrollbar |
				ImGuiWindowFlags_NoMove |
				ImGuiWindowFlags_NoResize;

			ImGui::SetNextWindowPos(ImVec2(1150, 80), ImGuiCond_Once);
			ImGui::SetNextWindowSize(ImVec2(280, 450), ImGuiCond_Once);
			ImGui::Begin("GroundTileset", nullptr, window_flags);

			GameMap* currentMap = levelEditor.GetCurrentMap();
			if (currentMap == nullptr) {
				ImGui::End();
				return;
			}

			GameMapGrounds* grounds = currentMap->GetGameMapGround();
			if (grounds == nullptr) {
				ImGui::End();
				return;
			}
			
			GameMapGroundTileset& tileset = grounds->GetTileset();
			auto texture = tileset.GetBaseColorTexture() != nullptr ? tileset.GetBaseColorTexture()->mTexture : nullptr;
			if (texture == nullptr) {
				ImGui::End();
				return;
			}

			auto& desc = texture->GetDesc();
			const size_t cellSize = 16;
			const size_t colCount = desc.mWidth / cellSize;
			const size_t rowCount = desc.mHeight / cellSize;
			const F32 invWidth = 1.0f / desc.mWidth;
			const F32 invHeight = 1.0f / desc.mHeight;

			size_t currentIndex = levelEditor.GetGroundTilesetIndex();
			size_t index = 0;
			for (int y = 0; y < rowCount; y++)
			{
				ImGui::NewLine();
				for (int x = 0; x < colCount; x++)
				{
					ImGui::SameLine();
					const F32 xOffset = (F32)(x * cellSize);
					const F32 yOffset = (F32)(y * cellSize);

					ImGui::PushID(index);
					if (ImGui::ImageButton(
						texture, 
						ImVec2(20, 20), 
						ImVec2(xOffset * invWidth, yOffset * invHeight),
						ImVec2((xOffset + cellSize) * invWidth, (yOffset + cellSize) * invHeight),
						currentIndex == index ? 2 : 0,
						ImVec4(0.0f, 0.0f, 0.0f, 0.0f)
					)) {
						levelEditor.SetCurrentGroundTilestIndex(index);
					}
					ImGui::PopID();

					index++;
				}
			}
			ImGui::End();
		}

		void ShowWallEditorWindow(LevelEditor& levelEditor)
		{

		}

		void ShowTipsWindow(LevelEditor& levelEditor)
		{
			auto hitLocalPos = levelEditor.GetCurrentPickLocalPos();
			if (hitLocalPos[0] < 0 || hitLocalPos[1] < 0 || hitLocalPos[2] < 0) {
				return;
			}

			ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar |
											ImGuiWindowFlags_NoScrollbar |
											ImGuiWindowFlags_NoMove |
											ImGuiWindowFlags_NoBackground |
											ImGuiWindowFlags_NoResize;

			ImGui::SetNextWindowPos(ImVec2(300, 80), ImGuiCond_Once);
			ImGui::SetNextWindowSize(ImVec2(300, 150), ImGuiCond_Once);
			ImGui::Begin("TipsWindow", nullptr, window_flags);

			I32 layer = levelEditor.GetCurrentLayer();
			ImGui::Text("Current layer = %d", layer);
			ImGui::Text("Local pos: x = %d, y = %d, z = %d", hitLocalPos[0], hitLocalPos[1], hitLocalPos[2]);
			ImGui::End();
		}

		void ShowNewMapWindow(LevelEditor& levelEditor)
		{
			if (ImGui::BeginPopupModal("New Map", NULL))
			{
				ImGui::SetNextItemWidth(100.0f);
				static char nameStr[128] = "";
				ImGui::InputText("name", nameStr, IM_ARRAYSIZE(nameStr));

				static I32x3 mapSize = { 0, 0, 0 };
				ImGui::InputInt3("Map Width/Height/Layer", mapSize.data());

				if (ImGui::Button("Confirm"))
				{
					levelEditor.NewMap(nameStr, mapSize[0], mapSize[1], mapSize[2]);
					ImGui::CloseCurrentPopup();
				}
				ImGui::SameLine();
				if (ImGui::Button("Close"))
					ImGui::CloseCurrentPopup();
				ImGui::EndPopup();
			}
		}

		void ShowEditorWindow(F32 deltaTime, LevelEditor& levelEditor)
		{
			ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar  |
											ImGuiWindowFlags_NoScrollbar |
											ImGuiWindowFlags_MenuBar |
											ImGuiWindowFlags_NoMove |
											ImGuiWindowFlags_NoResize;

			ImGui::SetNextWindowPos(ImVec2(500, 20), ImGuiCond_Once);
			ImGui::SetNextWindowSize(ImVec2(1000, 50), ImGuiCond_Once);
			ImGui::Begin("MapEditor", nullptr, window_flags);

			// menu bar
			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("File"))
				{
					if (ImGui::MenuItem("New Map")) { 
						ShowNewMapWindow(levelEditor);
					}
					if (ImGui::MenuItem("Load Map")) {
						GameWindow::ShowBrowseForFolder(
							"Select map folder",
							[&levelEditor](const std::string& filePath) {
								levelEditor.LoadMap(filePath);
							});
					}
					if (ImGui::MenuItem("Save Map")) { 
						GameWindow::ShowBrowseForFolder(
							"Select map folder",
							[&levelEditor](const std::string& filePath) {
								levelEditor.SaveMap(filePath);
							});
					}
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
			if (ImGui::Checkbox("Grid", &isGridVisible)) {
				levelEditor.SetDebugGridVisible(isGridVisible);
			}

			ImGui::End();

			ShowTipsWindow(levelEditor);

			switch (levelEditor.GetEditorMode())
			{
			case EditorMode_Ground:
				ShowGroundEditorWindow(levelEditor);
				break;
			case EditorMode_Wall:
				ShowWallEditorWindow(levelEditor);
				break;
			default:
				break;
			}
		}
	}

	void LevelEditor::InitializeEditorGUI(IMGUIStage& imguiStage)
	{
		imguiStage.RegisterCustomWindow([this](F32 deltaTime) {ShowEditorWindow(deltaTime, *this); });
	}
}