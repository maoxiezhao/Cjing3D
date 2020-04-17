#pragma once

#include "gui\guiInclude.h"
#include "guiRenderer.h"

namespace Cjing3D
{
	// IMGUIStage
	class IMGUIStage
	{
	public:
		using CustomWindowFunc = std::function<void()>;
		friend class GUIRenderer;

		IMGUIStage();
		~IMGUIStage();

		void Initialize();
		void Uninitialize();
		void Update(F32 deltaTime);
		void Render(GUIRenderer& renderer);

		void ShowBasicInfo(bool show) { mShowBasicInfo = show; }
		void ShowDetailInfo(bool show) { mShowDetailInfo = show; }
		bool IsShowBasicInfo()const { return mShowBasicInfo; }
		bool IsShowDetailInfo()const { return mShowDetailInfo; }

		void RegisterCustomWindow(CustomWindowFunc func);

	private:
		void InitializeImpl();
		void UpdateImpl(F32 deltaTime);
		void RenderImpl();

	private:
		bool mIsInitialized = false;
		bool mShowBasicInfo = true;
		bool mShowDetailInfo = true;

		std::vector <CustomWindowFunc> mRegisteredWindows;
	};
}