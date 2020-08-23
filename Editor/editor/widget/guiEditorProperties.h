#pragma once

#include "gui\guiEditor\guiEditorWidget.h"

namespace Cjing3D
{
	class EditorWidgetProperties : public EditorWidget
	{
	public:
		EditorWidgetProperties(IMGUIStage& imguiStage);

		void Update(F32 deltaTime)override;
	};
}