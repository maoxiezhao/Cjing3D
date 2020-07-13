#include "guiEditorSound.h"
#include "guiEditor.h"
#include "gui\guiEditor\guiEditorInclude.h"
#include "guiEditorScene.h"

namespace Cjing3D {
namespace Editor {


	bool bShowNewSoundWindow = false;
	void ShowNewSoundWindowImpl(F32 deltaTime)
	{
		if (bShowNewSoundWindow == false) return;

		ImGui::SetNextWindowPos(ImVec2(500, 100), ImGuiCond_Once);
		ImGui::SetNextWindowSize(ImVec2(350, 150), ImGuiCond_Once);
		ImGui::Begin("New Sound", &bShowNewSoundWindow);

		static char str0[128] = "new sound";
		ImGui::InputText("input name", str0, IM_ARRAYSIZE(str0));
		static F32x3 vec3f = { 0.0f, 0.0f, 0.0f };

		static bool is3DSound = false;
		ImGui::Checkbox("3D Sound", &is3DSound);

		ImGui::DragFloat3("position", vec3f.data(), 0.005f, -1000, 1000);
		static F32x3 color = { 1.0f, 1.0f, 1.0f };

		static std::string soundPath = "NULL";
		ImGui::Text(std::string("Sound File:" + soundPath).c_str());
		ImGui::SameLine();
		if (ImGui::Button("Load")) {
			soundPath = GetFileNameFromOpenFile("Sound File\0*.wav\0");
		}

		ImGui::NewLine();
		ImGui::SameLine();
		if (ImGui::Button("Create"))
		{
			Scene& scene = Scene::GetScene();
			scene.CreateSound(
				std::string(str0),
				soundPath,
				is3DSound,
				vec3f
			);
			bShowNewSoundWindow = false;
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			bShowNewSoundWindow = false;
		}

		ImGui::End();
	}

	void ShowSoundAttribute(SoundComponent* sound)
	{
		if (sound == nullptr) {
			return;
		}

		ImGui::Separator();
		if (!ImGui::CollapsingHeader("Sound", ImGuiTreeNodeFlags_DefaultOpen)) {
			return;
		}

		static std::string soundPath = "";
		ImGui::Text(std::string("Sound File:" + soundPath).c_str());
		ImGui::SameLine();
		if (ImGui::Button("Load")) 
		{
			soundPath = GetFileNameFromOpenFile("Sound File\0*.wav\0");
			sound->LoadSoundFromFile(soundPath);
		}

		if (sound->IsPlaying())
		{
			if (ImGui::Button("Stop")) {
				sound->Stop();
			}
		}
		else
		{
			if (ImGui::Button("Play")) {
				sound->Play();
			}
		}
		ImGui::SameLine();
		bool looped = sound->IsLooped();
		if (ImGui::Checkbox("Loop", &looped)) {
			sound->SetLooped(looped);
		}

		I32 volume = (I32)(sound->GetVolue() * 100.0f);
		if (ImGui::SliderInt("slider int2", &volume, 0, 255)) {
			sound->SetVolume((F32)volume / 100.0f);
		}
	}

	void ShowNewSoundWindow()
	{
		bShowNewSoundWindow = true;
	}

	void InitializeEditorSound(IMGUIStage& imguiStage)
	{
		imguiStage.RegisterCustomWindow(ShowNewSoundWindowImpl);
	}
}
}