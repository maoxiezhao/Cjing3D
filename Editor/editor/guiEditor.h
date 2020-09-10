#pragma once

#include "definitions\definitions.h"
#include "renderer\renderer.h"
#include "utils\signal\signal.h"

#include <functional>

namespace Cjing3D
{
	class EditorWidget;
	class GameWindow;

	class EditorStage
	{
	public:
		using CustomWindowFunc = std::function<void(F32)>;

		EditorStage(GameWindow& gameWindow);
		~EditorStage();

		void Initialize();
		void Uninitialize();
		void Render(CommandList cmd);
		void EndFrame();

		void RegisterCustomWindow(CustomWindowFunc func);
		void RegisterCustomWidget(std::shared_ptr<EditorWidget> widget);

		void SetVisible(bool isVisible) { mIsVisible = isVisible; }
		bool IsVisible()const { return mIsVisible; }
		void SetDemoVisible(bool visible) { mIsShowDemo = visible; }
		bool IsDemoVisible()const { return mIsShowDemo; }
		void SetDockingEnable(bool isDockingEnable);
		bool IsDockingEnable()const { return mIsDockingEnable; }

		Texture2D* GetViewportRenderTarget() {
			return &mViewportRenderTarget;
		}

		template<typename T>
		T* GetCustomWidget()
		{
			for (const auto& widget : mRegisteredWidgets) {
				if (T* widget_t = dynamic_cast<T*>(widget.get())) {
					return widget_t;
				}
			}
			return nullptr;
		}

	protected:
		void DockingBegin();
		void DockingEnd();
		void InitializeWidgets();
		void FixedUpdateImpl(F32 deltaTime);
		void CreateViewportRenderTarget(U32 width, U32 height);

	protected:
		bool mIsInitialized = false;
		bool mIsVisible = true;
		bool mIsNeedRender = false;
		bool mIsShowDemo = false;
		bool mIsDockingEnable = false;
		bool mIsDockingBegin = false;

		GameWindow& mGameWindow;
		Texture2D mViewportRenderTarget;
		ScopedConnection mResolutionChangedConn;

		std::vector<CustomWindowFunc> mRegisteredWindows;
		std::vector<std::shared_ptr<EditorWidget>> mRegisteredWidgets;
	};
}