#include "guiEditorProperties.h"
#include "guiEditorInclude.h"

namespace Cjing3D
{
	EditorWidgetProperties::EditorWidgetProperties(EditorStage& imguiStage) :
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