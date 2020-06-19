#pragma once

#include "gui\guiInclude.h"
#include "gui\guiWidgets\widgets.h"

namespace Cjing3D
{
	class GUIStage;
	class IMGUIStage;

	class GUIRenderer
	{
	public:
		GUIRenderer(GUIStage& guiStage);
		~GUIRenderer();

		void Update(F32 dt);
		void Render();
		void SetImGuiStage(IMGUIStage* imGuiStage);

	private:
		GUIStage& mGUIStage;
		IMGUIStage* mImGuiStage = nullptr;
	};
}