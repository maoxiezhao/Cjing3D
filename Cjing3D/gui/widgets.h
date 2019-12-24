#pragma once

#include "common\common.h"
#include "helper\stringID.h"
#include "system\component\transform.h"

namespace Cjing3D
{
	class GUIStage;

	class Widget : public TransformComponent
	{
	public:
		enum WidgetState
		{
			WidgetState_Idle,
			WidgetStage_Focus,
			WidgetStage_Active,
			WidgetStage_Deactive,
		};

		Widget(GUIStage& stage, const StringID& name = StringID::EMPTY);
		~Widget();

		virtual void Update(F32 dt);
		virtual void Render();
		virtual void RenderChilds();

		GUIStage& GetGUIStage();
		StringID GetName()const { return mName; }
		bool IsEnabled()const { return mEnabled; }
		bool IsVisible()const { return mVisible; }
		WidgetState GetStage()const { return mState; }
	private:
		virtual void RenderImpl() = 0;
		virtual void RenderChildsImpl() = 0;

	private:
		GUIStage& mStage;
		StringID mName;
		WidgetState mState = WidgetState::WidgetState_Idle;
		bool mEnabled = true;
		bool mVisible = true;

		F32x3 mPostition;
		F32x3 mSize;
	};
}