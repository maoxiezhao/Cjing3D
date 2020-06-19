#include "widgetHierarchy.h"
#include "gui\guiStage.h"

namespace Cjing3D
{
namespace Gui
{
	WidgetHierarchy::WidgetHierarchy(GUIStage& guiStage) :
		mGUIStage(guiStage)
	{
	}

	WidgetHierarchy::~WidgetHierarchy()
	{
	}

	void WidgetHierarchy::Initialize(U32x2 size)
	{
		// initialize root widget
		mRootWidget = std::make_shared<Widget>(mGUIStage, STRING_ID(root));
		mRootWidget->SetArea(Rect(
			0.0f, 0.0f,
			(F32)size[0], (F32)size[1]
		));
		mRootWidget->SetVisible(true);
	}

	void WidgetHierarchy::Uninitialize()
	{
	}

	void WidgetHierarchy::Update(F32 deltaTime)
	{
	}

	void WidgetHierarchy::FixedUpdate()
	{
	}

	void WidgetHierarchy::Render()
	{
	}

	void WidgetHierarchy::HandleInputEvents(const GUIInputEvent& inputEvent)
	{
	}
}
}