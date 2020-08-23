#pragma once

#include "gui\guiEditor\guiEditorWidget.h"

namespace Cjing3D
{
	class EditorWidgetMenu : public EditorWidget
	{
	public:
		EditorWidgetMenu(IMGUIStage& imguiStage);

		void Update(F32 deltaTime)override;
	};
}