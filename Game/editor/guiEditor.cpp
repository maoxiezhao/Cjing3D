#include "guiEditor.h"
#include "guiEditorLight.h"
#include "guiEditorProfier.h"
#include "guiEditorScene.h"
#include "guiEditorGUI.h"
#include "guiEditorAnimation.h"
#include "guiEditorRender.h"
#include "guiEditorHierarchy.h"
#include "guiEditorObject.h"
#include "guiEditorTerrain.h"
#include "guiEditorMaterial.h"
#include "guiEditorSound.h"

#include "gui\guiEditor\guiEditorInclude.h"

namespace Cjing3D {
namespace Editor {

	//////////////////////////////////////////////////////////////////////////////////////////////////

	bool bShowAboutWindow = false;
	void ShowAboutWindow(F32 deltaTime)
	{
		if (bShowAboutWindow == false) return;

		ImGui::SetNextWindowPos(ImVec2(10, 50), ImGuiCond_Once);
		ImGui::SetNextWindowSize(ImVec2(350, 200), ImGuiCond_Once);
		ImGui::Begin("About", &bShowAboutWindow);
		ImGui::Text("Cjing3D v%s", CjingVersion::GetVersion());
		ImGui::Text("By ZZZZY");
		ImGui::Separator();
		ImGui::Text("");
		ImGui::Text("F4-Show ImGUI Editor;");
		ImGui::Text("WASD-Move camera;  RB-Rotate camera");
		ImGui::Text("");
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", deltaTime, 1.0f / deltaTime);
		ImGui::End();
	}

	void ShowMainMenuBar(F32 deltaTime)
	{
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New Scene")) { Scene::GetScene().Clear(); }
				if (ImGui::MenuItem("Load Scene")) { LoadSceneFromOpenFile(); }
				if (ImGui::MenuItem("Save Scene")) { SaveSceneToOpenFile(); }
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Prop"))
			{
				if (ImGui::MenuItem("Animations")) { ShowAnimationWindow(); }
				if (ImGui::MenuItem("GUI")) {  ShowGUIWindow(); }
				if (ImGui::MenuItem("Render")) { ShowRenderWindow(); }
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Component"))
			{
				if (ImGui::MenuItem("Light")) { ShowNewLightWindow(); }
				if (ImGui::MenuItem("Sound")) { ShowNewSoundWindow(); }
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Profiler"))
			{
				if (ImGui::MenuItem("CPU Profiler")) { ShowCPUProfilerWindow(); }
				if (ImGui::MenuItem("GPU Profiler")) { ShowGPUProfilerWindow(); }
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Help"))
			{
				if (ImGui::MenuItem("About")) { bShowAboutWindow = true; }
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////

	void InitializeEditor(IMGUIStage& imguiStage)
	{
#ifndef _DISBALE_IMGUI_EDITOR_

		imguiStage.RegisterCustomWindow(ShowMainMenuBar);
		imguiStage.RegisterCustomWindow(ShowAboutWindow);

		InitializeEditorHierarchy(imguiStage);
		InitializeEditorProfiler(imguiStage);
		InitializeEditorLight(imguiStage);
		InitializeEditorGUI(imguiStage);
		InitializeEditorAnimation(imguiStage);
		InitializeEditorRender(imguiStage);
		InitializeEditorSound(imguiStage);
#endif
	}
}
}

