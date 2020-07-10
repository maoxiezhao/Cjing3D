#include "guiEditorAnimation.h"
#include "guiEditor.h"
#include "gui\guiEditor\guiEditorInclude.h"
#include "guiEditorTerrain.h"

namespace Cjing3D {
	namespace Editor {

		void InitializeEditorTerrain(IMGUIStage& imguiStage)
		{
		}

		void ShowTerrainAttribute(TerrainComponent* terrain)
		{
			if (terrain != nullptr)
			{
				I32 elevation = (I32)terrain->GetElevation();
				if (ImGui::SliderInt("Elevation", &elevation, 0, 200)) {
					terrain->SetElevation((U32)elevation);
				}
			}
		}

	}
}