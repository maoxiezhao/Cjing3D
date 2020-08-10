#pragma once

#include "editor\guiEditorInclude.h"

namespace Cjing3D
{
	class IMGUIStage;
	class ParticleComponent;

	namespace Editor
	{
		void InitializeEditorParticle(IMGUIStage& imguiStage);

		// public method
		void ShowParticleAttribute(ParticleComponent* particle);
		void ShowNewParticle();
	}
}