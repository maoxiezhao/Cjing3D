#pragma once

#include "common\common.h"

namespace Cjing3D
{
	class IMGUIStage;

	class GUIRenderer
	{
	public:
		GUIRenderer();
		~GUIRenderer();

		void Update(F32 dt);
		void Render();

		void SetImGuiStage(IMGUIStage* imGuiStage);

	private:
		IMGUIStage * mImGuiStage = nullptr;
	};
}