#pragma once

#include "gui\guiInclude.h"
#include "gui\guiWidgets\widgets.h"

namespace Cjing3D
{
class GUIStage;

namespace Gui
{
	class WidgetHierarchy
	{
	public:
		WidgetHierarchy(GUIStage& guiStage);
		~WidgetHierarchy();

		void Initialize(U32x2 size);
		void Uninitialize();
		void Update(F32 deltaTime);
		void FixedUpdate();
		void Render();
		void HandleInputEvents(const GUIInputEvent& inputEvent);

		WidgetPtr GetRootWidget() { return mRootWidget; }

	public:
		GUIStage& mGUIStage;
		WidgetPtr mRootWidget = nullptr;
	};
}
}