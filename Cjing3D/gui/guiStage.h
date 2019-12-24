#pragma once

#include "common\common.h"
#include "core\subSystem.hpp"
#include "gui\guiRenderer.h"
#include "gui\widgets.h"

namespace Cjing3D
{
	class GUIStage : public SubSystem
	{
	public:
		GUIStage(SystemContext& systemContext);
		~GUIStage();

		void Initialize();
		void Uninitialize();
		void Render();
		void Update(F32 deltaTime);

		GUIRenderer& GetGUIRenderer() { return mRenderer; }
		const GUIRenderer& GetGUIRenderer()const { return mRenderer; }
	private:
		GUIRenderer mRenderer;

	};
}