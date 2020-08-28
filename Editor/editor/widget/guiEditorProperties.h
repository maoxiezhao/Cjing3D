#pragma once

#include "guiEditorWidget.h"

namespace Cjing3D
{
	class EditorWidgetProperties : public EditorWidget
	{
	public:
		EditorWidgetProperties(EditorStage& imguiStage);

		void Update(F32 deltaTime)override;
	};
}