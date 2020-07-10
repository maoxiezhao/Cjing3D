#pragma once

#include "editor\guiEditorInclude.h"

namespace Cjing3D
{
	class IMGUIStage;
	class MaterialComponent;

	namespace Editor
	{
		void InitializeEditorMaterial(IMGUIStage& imguiStage);

		// public method
		void ShowMaterialAttribute(MaterialComponent* material);
	}
}