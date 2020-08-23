#include "guiEditorToolbar.h"

namespace Cjing3D
{
	EditorWidgetToolbar::EditorWidgetToolbar(IMGUIStage& imguiStage) :
		EditorWidget(imguiStage)
	{
		mTitleName = "ToolBar";
		mIsWindow = true;
		mWidgetFlags =
			ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoSavedSettings |
			ImGuiWindowFlags_NoScrollbar |
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoDocking;
	}

	void EditorWidgetToolbar::Update(F32 deltaTime)
	{
	}

	void EditorWidgetToolbar::PreBegin()
	{
		ImGuiContext& ctx = *ImGui::GetCurrentContext();
		mPos  = F32x2(ctx.Viewports[0]->Pos.x, ctx.Viewports[0]->Pos.y + 20.0f);
		mSize = F32x2(ctx.Viewports[0]->Size.x, ctx.NextWindowData.MenuBarOffsetMinVal.y + ctx.FontBaseSize + ctx.Style.FramePadding.y + 20.0f);

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 5));
	}

	void EditorWidgetToolbar::PostBegin()
	{
		ImGui::PopStyleVar();
	}
}
