#include "guiEditorRender.h"
#include "guiEditor.h"
#include "gui\guiEditor\guiEditorInclude.h"
#include "guiEditorScene.h"

namespace Cjing3D {
	namespace Editor {

		bool bShowRenderWindow = false;
		void ShowRenderWindow()
		{
			bShowRenderWindow = true;
		}

		//////////////////////////////////////////////////////////////////////////////////////

		void ShowPostprocess()
		{
			RenderPath* renderPath = Renderer::GetRenderPath();
			if (renderPath == nullptr) return;

			// tone mapping exposure
			RenderPath3D* renderPath3D = dynamic_cast<RenderPath3D*>(renderPath);
			if (renderPath3D == nullptr) {
				return;
			}

			F32 explore = renderPath3D->GetExposure();
			ImGui::SetNextItemWidth(100);
			if (ImGui::DragFloat("ToneMappingExposure", &explore, 0.1f, 0.1f, 10.0f)) {
				renderPath3D->SetExposure(explore);
			}

			bool fxaaEnable = renderPath3D->IsFXAAEnable();
			if (ImGui::Checkbox("FXAA", &fxaaEnable)) {
				renderPath3D->SetFXAAEnable(fxaaEnable);
			}

			// ao type
			int typeIndex = static_cast<int>(renderPath3D->GetAOType());
			const char* items[] = { "AOTYPE_DISABLE", "AOTYPE_SSAO" };
			if (ImGui::Combo("AO", &typeIndex, items, ARRAYSIZE(items))) {
				renderPath3D->SetAOType(static_cast<RenderPath3D::AOTYPE>(typeIndex));
			}
		}

		void ShowRenderAttribute()
		{
			RenderPath* renderPath = Renderer::GetRenderPath();
			if (renderPath == nullptr) return;

			RenderPath3D* renderPath3D = dynamic_cast<RenderPath3D*>(renderPath);
			if (renderPath3D == nullptr) return;

			// ambient light
			F32x3 ambientColor = Renderer::GetAmbientColor();
			F32* ambientColors = ambientColor.data();
			if (ImGui::ColorEdit3("color", ambientColors)) {
				Renderer::SetAmbientColor(ambientColor);
			}

			// sky box

			Scene& scene = Scene::GetScene();
			if (scene.mWeathers.Empty()) {
				ImGui::Text("Sky: NULL");
			}
			else
			{
				auto weather = scene.mWeathers[0];
				const std::string name = "Sky: " + weather->mSkyMapName;
				ImGui::Text(name.c_str());
			}
			ImGui::SameLine();
			if (ImGui::Button("Load sky"))
			{
				LoadSkyFromOpenFile();
			}
		}

		void ShowRenderWindowImpl(F32 deltaTime)
		{
			if (bShowRenderWindow == false) return;

			ImGui::SetNextWindowPos(ImVec2(500, 210), ImGuiCond_Once);
			ImGui::SetNextWindowSize(ImVec2(500, 260), ImGuiCond_Once);
			ImGui::Begin("GUI Animation", &bShowRenderWindow);

			if (ImGui::BeginTabBar("Properties", ImGuiTabBarFlags_None))
			{
				if (ImGui::BeginTabItem("Weather"))
				{
					ShowRenderAttribute();
					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("Postprocess"))
				{
					ShowPostprocess();
					ImGui::EndTabItem();
				}

				ImGui::EndTabBar();
			}

			ImGui::End();
		}

		void InitializeEditorRender(IMGUIStage& imguiStage)
		{
			imguiStage.RegisterCustomWindow(ShowRenderWindowImpl);
		}
	}
}