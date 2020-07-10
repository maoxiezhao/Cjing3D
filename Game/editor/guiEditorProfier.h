#pragma once

#include "editor\guiEditorInclude.h"

namespace Cjing3D
{
	class IMGUIStage;

	namespace Editor
	{
		void InitializeEditorProfiler(IMGUIStage& imguiStage);
		void ShowCPUProfilerWindow();
		void ShowGPUProfilerWindow();
	}
}