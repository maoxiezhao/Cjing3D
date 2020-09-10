#pragma once

#include "guiEditorWidget.h"

namespace Cjing3D
{
	class EditorWidgetMenu : public EditorWidget
	{
	public:
		EditorWidgetMenu(EditorStage& imguiStage);

		void Update(F32 deltaTime)override;
	};
}