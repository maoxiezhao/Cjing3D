#pragma once

#include "gui\guiInclude.h"
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
		void Render(GUIRenderer& renderer);

	private:
		void UpdateImpl(F32 deltaTime);
		void RenderImpl();

	private:
		bool mIsInitialized;
	};
}