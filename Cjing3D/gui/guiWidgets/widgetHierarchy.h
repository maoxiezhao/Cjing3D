#pragma once

#include "gui\guiInclude.h"
#include "gui\guiWidgets\widgets.h"
#include "gui\guiCore\guiDistributor.h"

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
		void RefreshWidgets();

		void CaptureFocusWidget(bool captured);
		WidgetPtr GetCurrentFocusdWidget();

		void AddWidget(WidgetPtr widget);
		void RemoveWidget(WidgetPtr widget);

		WidgetPtr GetRootWidget() { return nullptr; }
		
	public:
		GUIStage& mGUIStage;
		std::vector<WidgetPtr> mSubscribeRequests;
		std::vector<WidgetPtr> mWidgets;
		EventDistributor mEventDistributor;
	};
}
}