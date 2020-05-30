#pragma once

#include "common\common.h"
#include "system\ecsSystem.h"

namespace Cjing3D
{
	class IMGUIStage;
	
	namespace Editor
	{
		void InitializeEditorLight(IMGUIStage& imguiStage);
		void SetCurrentLight(ECS::Entity light);
		void ShowNewLightWindow();
	}
}