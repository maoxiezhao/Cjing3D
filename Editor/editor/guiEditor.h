#pragma once

#include "system\ecsSystem.h"
#include "gui\imguiStage.h"

namespace Cjing3D
{
	class EditorStage : public IMGUIStage
	{
	public:
		EditorStage(GUIRenderer& renderer);
		~EditorStage();

	protected:
		virtual void InitializeImpl();
		virtual void DockingBegin();
		virtual void DockingEnd();
	};
}