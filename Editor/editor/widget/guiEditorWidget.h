#pragma once

#include "definitions\definitions.h"
#include "editor\guiEditor.h"

struct ImGuiWindow;

namespace Cjing3D
{
	class EditorWidget
	{
	public:
		EditorWidget(EditorStage& imguiStage);

		virtual void Initialize();
		virtual void Update(F32 deltaTime);
		virtual void Uninitialize();

		bool Begin();
		void End();

		bool IsVisible()const { return mIsVisible; }
		void SetVisible(bool isVisible) { mIsVisible = isVisible; }
		F32 GetHeight()const { return mHeight; }

	protected:
		virtual void PreBegin();
		virtual void PostBegin();

	protected:
		EditorStage& mStage;
		bool mIsVisible = true;
		bool mIsBegun = false;
		bool mIsWindow = true;
		I32 mWidgetFlags = 0;
		F32x2 mPos = F32x2(-1.0f, -1.0f);
		F32x2 mSize = F32x2(-1.0f, -1.0f);
		F32 mHeight = 0.0f;
		std::string mTitleName;
		ImGuiWindow* mWindow = nullptr;
		U32 mPushedStyleVar = 0;
	};
}