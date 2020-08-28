#pragma once

#include "guiEditorWidget.h"

namespace Cjing3D
{
	class EditorWidgetEntityList : public EditorWidget
	{
	public:
		EditorWidgetEntityList(EditorStage& imguiStage);

		void Update(F32 deltaTime)override;
	};
}