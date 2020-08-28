#include "guiEditorMenuBar.h"
#include "guiEditorHelper.h"
#include "common\version.h"
#include "guiEditorInclude.h"

namespace Cjing3D
{
	namespace 
	{	
		bool bShowAboutWindow = false;
		bool bShowMousePicking = false;
	}
	//////////////////////////////////////////////////////////////////////////////////////////////////

	void ShowAboutWindow(F32 deltaTime)
	{
		if (bShowAboutWindow == false) return;

		ImGui::Begin("About", &bShowAboutWindow, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking);
		ImGui::Text("Copyright (c) 2019-2020 ZZZY");
		ImGui::Text("GitHub:https://github.com/maoxiezhao/Cjing3D.git");
		ImGui::Text("Cjing3D Engine Version v%s", CjingVersion::GetVersionString());
		ImGui::Text("");
		ImGui::Separator();
		ImGui::Text("License");
		ImGui::Text("MIT License");
		ImGui::Text("Permission is hereby granted, free of charge, to any person obtaining a copy");
		ImGui::Text("of this softwareand associated documentation files(the \"Software\"), to deal");
		ImGui::Text("in the Software without restriction, including without limitation the rights");
		ImGui::Text("to use, copy, modify, merge, publish, distribute, sublicense, and /or sell");
		ImGui::Text("copies of the Software, and to permit persons to whom the Software is");
		ImGui::Text("furnished to do so, subject to the following conditions :");
		ImGui::Text("The above copyright noticeand this permission notice shall be included in all");
		ImGui::Text("copies or substantial portions of the Software.");
		ImGui::Text("THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR");
		ImGui::Text("IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,");
		ImGui::Text("FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE");
		ImGui::Text("AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER");
		ImGui::Text("LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,");
		ImGui::Text("OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE");
		ImGui::Text("SOFTWARE.");
		ImGui::End();
	}


	EditorWidgetMenu::EditorWidgetMenu(EditorStage& imguiStage) :
		EditorWidget(imguiStage)
	{
		mTitleName = "MenuBar";
		mIsWindow = false;
	}

	void EditorWidgetMenu::Update(F32 deltaTime)
	{
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New  Scene")) { Scene::GetScene().Clear(); }
				if (ImGui::MenuItem("Load Scene")) { EditorHelper::LoadSceneFromOpenFile(); }
				if (ImGui::MenuItem("Save Scene")) { EditorHelper::SaveSceneToOpenFile(); }
				if (ImGui::MenuItem("Exit"))       { GetGlobalContext().GetEngine()->SetIsExiting(true); }
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Help"))
			{
				bool demoVisible = mStage.IsDemoVisible();
				if (ImGui::MenuItem("Imgui Demo", nullptr, &demoVisible)) { mStage.SetDemoVisible(demoVisible); }
				if (ImGui::MenuItem("About")) { bShowAboutWindow = true; }
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}

		ShowAboutWindow(deltaTime);
	}
}