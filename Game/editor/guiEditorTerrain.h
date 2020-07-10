#pragma once

#include "editor\guiEditorInclude.h"

namespace Cjing3D
{
	class IMGUIStage;
	class TerrainComponent;

	namespace Editor
	{
		void InitializeEditorTerrain(IMGUIStage& imguiStage);

		// public method
		void ShowTerrainAttribute(TerrainComponent* terrain);
	}
}