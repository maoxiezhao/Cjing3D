#pragma once

#include "editor\guiEditorInclude.h"

namespace Cjing3D
{
	class IMGUIStage;
	class ObjectComponent;

	namespace Editor
	{
		void InitializeEditorObject(IMGUIStage& imguiStage);

		// public method
		void ShowObjectAttribute(ObjectComponent* object);
	}
}