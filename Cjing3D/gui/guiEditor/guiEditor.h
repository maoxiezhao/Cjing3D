#pragma once

#include "common\common.h"

namespace Cjing3D
{
	class IMGUIStage;
	class NameComponent;
	class TransformComponent;

	namespace Editor
	{
		void InitializeEditor(IMGUIStage& imguiStage);
	
		void ShowNameComponentAttribute(NameComponent* name);
		void ShowTransformAttribute(TransformComponent* transform);
	}
}