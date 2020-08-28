#pragma once

#include "definitions\definitions.h"

#include <functional>

namespace Cjing3D
{
	class EditorWidget;

	class EditorStage
	{
	public:
		using CustomWindowFunc = std::function<void(F32)>;

		EditorStage();
		~EditorStage();

		void Initialize();
		void Uninitialize();
		void FixedUpdate();
		void Render();
		void EndFrame();

		void RegisterCustomWindow(CustomWindowFunc func);
		void RegisterCustomWidget(std::shared_ptr<EditorWidget> widget);

		void SetVisible(bool isVisible) { mIsVisible = isVisible; }
		bool IsVisible()const { return mIsVisible; }
		void SetDemoVisible(bool visible) { mIsShowDemo = visible; }
		bool IsDemoVisible()const { return mIsShowDemo; }
		void SetDockingEnable(bool isDockingEnable);
		bool IsDockingEnable()const { return mIsDockingEnable; }

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
		void InitializeImpl();
		void FixedUpdateImpl(F32 deltaTime);

	protected:
		bool mIsInitialized = false;
		bool mIsVisible = true;
		bool mIsNeedRender = false;
		bool mIsShowDemo = false;
		bool mIsDockingEnable = false;
		bool mIsDockingBegin = false;
		int mConfigFlag = 0;

		std::vector<CustomWindowFunc> mRegisteredWindows;
		std::vector<std::shared_ptr<EditorWidget>> mRegisteredWidgets;
	};
}