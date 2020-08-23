#pragma once

#include "gui\guiEditor\guiEditorWidget.h"

namespace Cjing3D
{
	class EditorWidgetEntityList : public EditorWidget
	{
	public:
		EditorWidgetEntityList(IMGUIStage& imguiStage);

		void Update(F32 deltaTime)override;
	};
}