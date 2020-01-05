#pragma once

#include "gui\guiInclude.h"
#include "gui\widgets.h"

namespace Cjing3D
{
	class GUIStage;

	class WidgetManager
	{
	public:
		WidgetManager(GUIStage& guiStage);
		~WidgetManager();

		void Initialize();
		void Uninitialize();

	private:
		GUIStage& mGUIStage;
	};
} 