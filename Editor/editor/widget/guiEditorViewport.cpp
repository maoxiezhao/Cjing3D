#include "guiEditorViewport.h"
#include "guiEditorInclude.h"

namespace Cjing3D
{
	EditorWidgetViewport::EditorWidgetViewport(EditorStage& imguiStage) :
		EditorWidget(imguiStage)
	{
		mTitleName = "Viewport";
		mSize = F32x2(994.0f, 710.0f);
		mIsWindow = true;
		mWidgetFlags =
			ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoScrollbar;
	}

	void EditorWidgetViewport::Update(F32 deltaTime)
	{
		if (!Renderer::IsInitialized()) {
			return;
		}

		F32 width  = (F32)(ImGui::GetWindowContentRegionMax().x - ImGui::GetWindowContentRegionMin().x);
		F32 height = (F32)(ImGui::GetWindowContentRegionMax().y - ImGui::GetWindowContentRegionMin().y);

		ImGui::Image(
			static_cast<ImTextureID>(mStage.GetViewportRenderTarget()),
			ImVec2(width, height),
			ImVec2(0, 0),
			ImVec2(1, 1),
			ImColor(255, 255, 255, 255),
			ImColor(50, 127, 166, 255)
		);
	}
}