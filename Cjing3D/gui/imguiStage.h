#pragma once

#include "gui\guiInclude.h"
#include "guiRenderer.h"

namespace Cjing3D
{
	// IMGUIStage
	class IMGUIStage
	{
	public:
		using CustomWindowFunc = std::function<void(F32)>;
		friend class GUIRenderer;

		IMGUIStage();
		~IMGUIStage();

		void Initialize();
		void Uninitialize();
		void Update(F32 deltaTime);
		void Render(GUIRenderer& renderer);

		void RegisterCustomWindow(CustomWindowFunc func);

		void SetVisible(bool isVisible) { mIsVisible = isVisible; }
		bool IsVisible()const { return mIsVisible; }

	private:
		void InitializeImpl();
		void UpdateImpl(F32 deltaTime);
		void RenderImpl();

	private:
		bool mIsInitialized = false;
		bool mIsVisible = true;

		std::vector <CustomWindowFunc> mRegisteredWindows;
	};
}