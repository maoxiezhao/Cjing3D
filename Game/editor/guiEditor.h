#pragma once

#include "editor\guiEditorInclude.h"

namespace Cjing3D
{
	class IMGUIStage;

	namespace Editor
	{
		void InitializeEditor();
		void UpdateEditor();
		void UninitializeEditor();
		IMGUIStage& GetImGUIStage();
	}
}