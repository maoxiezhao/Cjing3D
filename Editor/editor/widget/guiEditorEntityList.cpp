#include "guiEditorEntityList.h"

namespace Cjing3D
{
	EditorWidgetEntityList::EditorWidgetEntityList(IMGUIStage& imguiStage) :
		EditorWidget(imguiStage)
	{
		mTitleName = "EntityList";
		mIsWindow = true;
		mWidgetFlags =
			ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_HorizontalScrollbar;
	}

	void EditorWidgetEntityList::Update(F32 deltaTime)
	{
	}
}