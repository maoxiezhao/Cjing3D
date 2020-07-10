#pragma once

#include "editor\guiEditorInclude.h"

namespace Cjing3D
{
	class IMGUIStage;
	class NameComponent;
	class TransformComponent;

	namespace Editor
	{
		void InitializeEditorHierarchy(IMGUIStage& imguiStage);

		// public method
		void ShowTransformAttributes(TransformComponent* transform);
		void ShowNameComponentAttribute(ECS::Entity entity);
	}
}