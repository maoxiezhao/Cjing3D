#pragma once

#include "editor\guiEditorInclude.h"

namespace Cjing3D
{
	class IMGUIStage;
	class LightComponent;

	namespace Editor
	{
		void InitializeEditorLight(IMGUIStage& imguiStage);

		// public method
		void ShowLightAttribute(LightComponent* light);
		void ShowNewLightWindow();
	}
}