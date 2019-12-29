#pragma once

#include "gui\guiInclude.h"
#include "system\component\transform.h"
#include "utils\treeNode.h"
#include "utils\geometry.h"

namespace Cjing3D
{
	class GUIStage;

	class Widget : public TransformComponent, public TreeNode<Widget>
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

		F32x2 TransfromToLocalCoord(const F32x2 point)const;
		bool HistTest(const F32x2& point)const;

		void SetArea(const Rect& rect);

		void SetEnabled(bool enabled) { mIsEnabled = enabled; }
		bool IsEnabled()const { return mIsEnabled; }
		void SetVisible(bool visible) { mIsVisible = visible; }
		bool IsVisible()const { return mIsVisible; }
		void SetWidgetType(WidgetType type) { mType = type; }
		WidgetType GetWidgetType()const { return mType; }
		WidgetState GetStage()const { return mState; }

		GUIStage& GetGUIStage();

	private:
		virtual void RenderImpl() {};
		virtual void RenderChildsImpl() {};

	protected:
		virtual void onParentChanged(Widget* old_parent);
		virtual void onChildAdded(std::shared_ptr<Widget>& node);
		virtual void onChildRemoved(std::shared_ptr<Widget>& node);

	private:
		GUIStage& mStage;
		WidgetState mState = WidgetState::WidgetState_Idle;
		bool mIsEnabled = true;
		bool mIsVisible = false;
		bool mIsIgnoreInputEvent = false;

		F32x3 mPostition;
		F32x3 mSize;
		Rect mArea;
		WidgetType mType = WidgetType::WidgetType_BaseWidget;
	};
	using WidgetPtr = std::shared_ptr<Widget>;
}