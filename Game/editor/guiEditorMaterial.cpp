#include "guiEditorMaterial.h"
#include "guiEditor.h"
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

			bool castingShadow = material->IsCastingShadow();
			if (ImGui::Checkbox("castShadow", &castingShadow)) {
				material->SetCastShadow(castingShadow);

				material->SetIsDirty(true);
			}
		}

	}
}