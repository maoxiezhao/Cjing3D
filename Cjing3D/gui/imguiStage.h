#pragma once

#include "common\common.h"
#include "guiRenderer.h"

namespace Cjing3D
{
	// IMGUIStage
	class IMGUIStage
	{
	public:
		friend class GUIRenderer;

		IMGUIStage();
		~IMGUIStage();

		void Initialize();
		void Uninitialize();
		void Update(F32 deltaTime);
		void Render();

	private:
		void RenderImpl();

	private:
		bool mIsInitialized;
	};
}