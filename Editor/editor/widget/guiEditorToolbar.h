#pragma once

#include "guiEditorWidget.h"

namespace Cjing3D
{
	class EditorWidgetToolbar : public EditorWidget
	{
	public:
		EditorWidgetToolbar(EditorStage& imguiStage);

		void Update(F32 deltaTime)override;

	protected:
		virtual void PreBegin();
		virtual void PostBegin();
	};
}