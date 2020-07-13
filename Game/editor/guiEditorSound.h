#pragma once

#include "editor\guiEditorInclude.h"

namespace Cjing3D
{
	class IMGUIStage;
	class SoundComponent;
	class Scene;

	namespace Editor
	{
		void InitializeEditorSound(IMGUIStage& imguiStage);

		// public method
		void ShowSoundAttribute(SoundComponent* sound);
		void ShowNewSoundWindow();
	}
}