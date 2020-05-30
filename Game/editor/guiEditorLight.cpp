#include "guiEditorLight.h"
#include "guiEditor.h"

#include "gui\guiEditor\guiEditorInclude.h"

namespace Cjing3D {
namespace Editor {

	Entity currentLight = INVALID_ENTITY;
	void ShowLightAttribute(F32 deltaTime)
	{
		if (currentLight == INVALID_ENTITY) return;

		ImGui::SetNextWindowPos(ImVec2(1070, 20), ImGuiCond_Once);
		ImGui::SetNextWindowSize(ImVec2(350, 360), ImGuiCond_Once);
		ImGui::Begin("Light attributes");

		Scene& scene = Scene::GetScene();
		// name
		auto name = scene.mNames.GetComponent(currentLight);
		ShowNameComponentAttribute(name);

		// transform
		auto transform = scene.mTransforms.GetComponent(currentLight);
		ShowTransformAttribute(transform);

		// light 
		auto light = scene.mLights.GetComponent(currentLight);
		if (light != nullptr)
		{
			ImGui::Spacing();
			ImGui::Text("Light");

			F32 color[3] = { light->mColor[0], light->mColor[1], light->mColor[2] };
			if (ImGui::ColorEdit3("color", color))
			{
				light->mColor[0] = color[0];
				light->mColor[1] = color[1];
				light->mColor[2] = color[2];
			}

			F32 energy = light->mEnergy;
			if (ImGui::DragFloat("energy", &energy, 0.1f, 0.0f, 200.0f))
			{
				light->mEnergy = energy;
			}

			F32 range = light->mRange;
			if (ImGui::DragFloat("range", &range,  0.1f, 0.0f, 200.0f))
			{
				light->mRange = range;
			}

			bool castingShadow = light->IsCastShadow();
			if (ImGui::Checkbox("castShadow", &castingShadow)) {
				light->SetCastShadow(castingShadow);
			}

			F32 shadowBias = light->mShadowBias;
			if (ImGui::DragFloat("shadowBias", &shadowBias, 0.00001f, 0.0f, 1.0f, "%.5f"))
			{
				light->mShadowBias = shadowBias;
			}

			int typeIndex = static_cast<int>(light->GetLightType());
			const char* items[] = { "LightType_Directional", "0LightType_Point", "0LightType_Spot" };
			if (ImGui::Combo("LightType", &typeIndex, items, ARRAYSIZE(items)))
			{
				light->SetLightType(static_cast<LightComponent::LightType>(typeIndex));
			}
		}

		ImGui::End();
	}

	bool bShowNewLightWindow = false;
	void ShowNewLighWindowImpl(F32 deltaTime)
	{
		if (bShowNewLightWindow == false) return;

		ImGui::SetNextWindowPos(ImVec2(500, 100), ImGuiCond_Once);
		ImGui::SetNextWindowSize(ImVec2(350, 200), ImGuiCond_Once);
		ImGui::Begin("New Light", &bShowNewLightWindow);

		static char str0[128] = "test light";
		ImGui::InputText("input name", str0, IM_ARRAYSIZE(str0));
		static F32x3 vec3f = { 0.0f, 0.0f, 0.0f};
		ImGui::DragFloat3("position", vec3f.data(), 0.005f, -1000, 1000);
		static F32x3 color = { 1.0f, 1.0f, 1.0f };
		ImGui::ColorEdit3("color", color.data());
		static F32 energy = 1.0f;
		ImGui::DragFloat("energy", &energy, 0.1f, 0.0f, 200.0f);
		static F32 range = 10.0f;
		ImGui::DragFloat("range", &range, 0.1f, 0.0f, 200.0f);
		ImGui::NewLine();
		ImGui::SameLine(100);
		if (ImGui::Button("Create"))
		{
			Scene& scene = Scene::GetScene();
			scene.CreateEntityLight(
				std::string(str0),
				vec3f,
				color,
				energy,
				range
			);
			bShowNewLightWindow = false;
		}
		ImGui::SameLine(200);
		if (ImGui::Button("Cancel"))
		{
			bShowNewLightWindow = false;
		}
		
		ImGui::End();
	}

	void InitializeEditorLight(IMGUIStage& imguiStage)
	{
		imguiStage.RegisterCustomWindow(ShowLightAttribute);
		imguiStage.RegisterCustomWindow(ShowNewLighWindowImpl);
	}

	void SetCurrentLight(ECS::Entity light)
	{
		currentLight = light;
	}

	void ShowNewLightWindow()
	{
		bShowNewLightWindow = true;
	}

}
}