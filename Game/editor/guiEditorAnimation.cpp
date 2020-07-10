#include "guiEditorAnimation.h"
#include "guiEditor.h"
#include "gui\guiEditor\guiEditorInclude.h"

namespace Cjing3D {
namespace Editor {

	bool bShowAnimationAttribute = false;
	void ShowAnimationAttribute(F32 deltaTime)
	{
		if (bShowAnimationAttribute == false) return;

		ImGui::SetNextWindowPos(ImVec2(1070, 20), ImGuiCond_Once);
		ImGui::SetNextWindowSize(ImVec2(350, 160), ImGuiCond_Once);
		ImGui::Begin("Animations attributes", &bShowAnimationAttribute);

		Scene& scene = Scene::GetScene();
		auto& animations = scene.mAnimations;
		if (animations.GetCount() <= 0) {
			ImGui::End();
			return;
		}

		// animation list
		static int animationIndex = 0;
		std::string animationNameList = "\0";

		for (int index = 0; index < animations.GetCount(); index++)
		{
			Entity entity = animations.GetEntityByIndex(index);
			std::string nodeName = "Entity ";
			auto nameComponent = scene.mNames.GetComponent(entity);
			if (nameComponent != nullptr) {
				nodeName = nodeName + " " + nameComponent->GetString();
			}
			else {
				nodeName = nodeName + std::to_string(entity);
			}

			animationNameList = animationNameList + nodeName + '\0';
		}

		ImGui::Combo("Animations", &animationIndex, animationNameList.c_str(), 20);

		if (animationIndex >= 0 && animationIndex < animations.GetCount()) {
			auto animation = animations.GetComponentByIndex(animationIndex);
			if (animation != nullptr)
			{
				bool looped = animation->IsLooped();
				if (ImGui::Checkbox("Loop", &looped)) {
					animation->SetIsLooped(looped);
				}
				ImGui::SameLine(100);
				if (ImGui::Button("Play")) {
					if (animation->IsPlaying())
					{
						animation->Pause();
					}
					else
					{
						animation->Play();
					}
				}
				ImGui::SameLine(150);
				if (ImGui::Button("Stop")) {
					animation->Stop();
				}
			}
		}

		ImGui::End();
	}


	bool bShowAnimationWindow = false;
	void ShowAnimationWindow()
	{
		bShowAnimationWindow = true;
		bShowAnimationAttribute = true;
	}

	void ShowAnimationWindowImpl(F32 deltaTime)
	{
		if (bShowAnimationWindow == false) return;

		ImGui::SetNextWindowPos(ImVec2(295, 510), ImGuiCond_Once);
		ImGui::SetNextWindowSize(ImVec2(1100, 260), ImGuiCond_Once);
		ImGui::Begin("GUI Animation", &bShowAnimationWindow);


		ImGui::End();
	}

	void InitializeEditorAnimation(IMGUIStage& imguiStage)
	{
		imguiStage.RegisterCustomWindow(ShowAnimationWindowImpl);
		imguiStage.RegisterCustomWindow(ShowAnimationAttribute);
	}
}
}