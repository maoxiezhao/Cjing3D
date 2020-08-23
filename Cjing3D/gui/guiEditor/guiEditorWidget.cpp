#include "guiEditorWidget.h"
#include "imgui_internal.h"

namespace Cjing3D
{
	EditorWidget::EditorWidget(IMGUIStage& imguiStage) :
		mStage(imguiStage)
	{
	}

	void EditorWidget::Initialize()
	{
	}

	void EditorWidget::Update(F32 deltaTime)
	{
	}

	void EditorWidget::Uninitialize()
	{
	}

	bool EditorWidget::Begin()
	{
		if (mIsBegun) {
			return false;
		}

		if (!mIsVisible) {
			return false;
		}

		// 如果Widget不是window,则直接返回true，也不设置mIsBegun
		if (!mIsWindow) {
			return true;
		}

		PreBegin();

		if (mPos[0] != -1.0f && mPos[1] != -1.0f) {
			ImGui::SetNextWindowPos(ImVec2(mPos[0], mPos[1]));
		}

		if (mSize[0] != -1.0f && mSize[1] != -1.0f) {
			ImGui::SetNextWindowSize(ImVec2(mSize[0], mSize[1]));
		}

		if (!ImGui::Begin(mTitleName.c_str(), &mIsVisible, mWidgetFlags)) {
			return false;
		}

		mIsBegun = true;
		mWindow = ImGui::GetCurrentWindow();
		mHeight = ImGui::GetWindowHeight();

		PostBegin();

		return true;
	}

	void EditorWidget::End()
	{
		if (!mIsBegun) {
			return;
		}

		ImGui::End();
		mIsBegun = false;

		ImGui::PopStyleVar(mPushedStyleVar);
		mPushedStyleVar = 0;
	}

	void EditorWidget::PreBegin()
	{
	}

	void EditorWidget::PostBegin()
	{
	}
}