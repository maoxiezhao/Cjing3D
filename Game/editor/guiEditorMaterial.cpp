#include "guiEditorMaterial.h"
#include "guiEditor.h"
#include "guiEditorScene.h"
#include "gui\guiEditor\guiEditorInclude.h"

namespace Cjing3D {
	namespace Editor {

		void InitializeEditorMaterial(IMGUIStage& imguiStage)
		{
		}

		void ShowMaterialAttribute(MaterialComponent* material)
		{
			if (material == nullptr) {
				return;
			}
	
			ImGui::Separator();
			ImGui::Text("Material");
			ImGui::Text("BaseColor");
			F32 color[4] = { material->mBaseColor[0], material->mBaseColor[1], material->mBaseColor[2], material->mBaseColor[3] };
			if (ImGui::ColorEdit4("baseColor", color))
			{
				material->mBaseColor[0] = color[0];
				material->mBaseColor[1] = color[1];
				material->mBaseColor[2] = color[2];
				material->mBaseColor[3] = color[3];

				material->SetIsDirty(true);
			}

			std::string baseColorMap = material->mBaseColorMapName;
			ImGui::Text("BaseColorMap");
			if (ImGui::Button(baseColorMap.c_str(), ImVec2(180, 0)))
			{
				auto filePath = GetFileNameFromOpenFile("Texture File\0*.dds;*.png\0");
				if (!filePath.empty()) {
					material->LoadBaseColorMap(filePath);
				}
			}

			std::string normalMap = material->mNormalMapName;
			ImGui::Text("NormalMapName");
			if (ImGui::Button(normalMap.c_str(), ImVec2(180, 0)))
			{
				auto filePath = GetFileNameFromOpenFile("Texture File\0*.dds;*.png\0");
				if (!filePath.empty()) {
					material->LoadNormalMap(filePath);
				}
			}

			std::string surfaceMapName = material->mSurfaceMapName;
			ImGui::Text("SurfaceMapName");
			if (ImGui::Button(surfaceMapName.c_str(), ImVec2(180, 0)))
			{
				auto filePath = GetFileNameFromOpenFile("Texture File\0*.dds;*.png\0");
				if (!filePath.empty()) {
					material->LoadSurfaceMap(filePath);
				}
			}

			bool castingShadow = material->IsCastingShadow();
			if (ImGui::Checkbox("castShadow", &castingShadow)) {
				material->SetCastShadow(castingShadow);

				material->SetIsDirty(true);
			}
		}

	}
}