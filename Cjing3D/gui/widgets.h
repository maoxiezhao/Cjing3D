#pragma once

#include "gui\guiInclude.h"
#include "utils\treeNode.h"
#include "utils\geometry.h"
#include "helper\enumInfo.h"

#include "utils\tinyxml2\tinyxml2.h"

namespace Cjing3D
{
	class GUIStage;

	class Widget : public TreeNode<Widget>
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

		virtual void InitProperties(tinyxml2::XMLElement& element);
		virtual void Update(F32 dt);
		virtual void Render(const F32x2& offset);

		F32x2 TransfromToLocalCoord(const F32x2 point)const;
		bool HistTest(const F32x2& point)const;

		void SetArea(const Rect& rect);
		Rect GetArea()const { return mArea; }
		void SetEnabled(bool enabled) { mIsEnabled = enabled; }
		bool IsEnabled()const { return mIsEnabled; }
		void SetVisible(bool visible) { mIsVisible = visible; }
		bool IsVisible()const { return mIsVisible; }

		WidgetState GetStage()const { return mState; }
		GUIStage& GetGUIStage();

		static WidgetType GetWidgetType() { 
			return WidgetType::WidgetType_BaseWidget; 
		}

		static const StringID GetWidgetTypeStr() {
			WidgetType widgetType = GetWidgetType();

			const std::string widgetTypeStr = EnumToString(widgetType);
			if (widgetTypeStr.empty()) {
				return StringID::EMPTY;
			}

			return StringID(widgetTypeStr);
		}

	private:
		virtual void RenderImpl(const Rect& destRect);

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

		Rect mArea;

		Color4 mDebugColor;
	};
	using WidgetPtr = std::shared_ptr<Widget>;

	// register enum infos
	template<>
	struct EnumInfoTraits<WidgetType>
	{
		static const std::string enumName;
		static const EnumInfo<WidgetType>::EnumType enumInfos;
	};
}