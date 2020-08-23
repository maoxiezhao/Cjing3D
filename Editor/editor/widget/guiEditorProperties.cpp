#include "guiEditorProperties.h"

namespace Cjing3D
{
	EditorWidgetProperties::EditorWidgetProperties(IMGUIStage& imguiStage) :
		EditorWidget(imguiStage)
	{
		mTitleName = "Properties";
		mIsWindow = true;
		mWidgetFlags =
			ImGuiWindowFlags_NoCollapse ;
	}

	void EditorWidgetProperties::Update(F32 deltaTime)
	{
	}
}