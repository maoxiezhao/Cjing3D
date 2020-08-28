#pragma once

#include "guiEditorWidget.h"

namespace Cjing3D
{
	class EditorWidgetViewport : public EditorWidget
	{
	public:
		EditorWidgetViewport(EditorStage& imguiStage);

		void Update(F32 deltaTime)override;
	};
}