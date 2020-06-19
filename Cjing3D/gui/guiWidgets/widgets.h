#pragma once

#include "gui\guiInclude.h"
#include "utils\treeNode.h"
#include "utils\geometry.h"
#include "helper\enumInfo.h"
#include "scripts\luaContext.h"

#include "gui\guiCore\guiDispatcher.h"

#include <set>

namespace Cjing3D
{
class GUIStage;
class GUIRenderer;

namespace Gui {

	struct GUIScriptEventHandlers
	{
		static const StringID OnLoaded;
		static const StringID OnUnloaded;

		static std::set<StringID> registeredScriptEventHandlers;
	};

	class Widget : public TreeNode<Widget>, public Gui::Dispatcher
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
		virtual ~Widget();

		virtual void OnLoaded();
		virtual void OnUnloaded();
		virtual void Update(F32 dt);
		virtual void FixedUpdate();
		virtual void Render(const F32x2& offset);

		GUIRenderer& GetGUIRenderer();
		GUIStage& GetGUIStage();

		bool HitTest(const F32x2& point)const;
		F32x2 TransfromToLocalCoord(const F32x2 point)const;

		// script handler
		LuaRef GetScriptHandler();
		void SetScriptHandler(LuaRef handler);
		void AddScriptEventHandler(const StringID& eventName, const std::string& handler);
		void CallScriptEventHandler(const StringID& eventName);
		void CallScriptEventHandlerWithVariants(const StringID& eventName, VariantArray variants);

		// basic status
		WidgetState GetStage()const { return mState; }
		void SetPos(const F32x2 pos);
		F32x2 GetPos()const;
		void SetSize(const F32x2 size);
		F32x2 GetSize()const;
		void SetArea(const Rect& rect);
		Rect GetArea()const { return mArea; }
		void SetEnabled(bool enabled) { mIsEnabled = enabled; }
		bool IsEnabled()const { return mIsEnabled; }
		void SetVisible(bool visible) { mIsVisible = visible; };
		bool IsVisible()const { return mIsVisible; }
		bool IsRoot()const { return  mIsRoot; }
		void SetIsRoot(bool isRoot) { mIsRoot = isRoot; }

		virtual WidgetType GetSelfWidgetType() const {
			return Widget::GetWidgetType();
		}

		static WidgetType GetWidgetType() {
			return WidgetType::WidgetType_BaseWidget;
		}

		std::shared_ptr<Widget> FindChildByName(const StringID& name);
		std::shared_ptr<Widget> GetChildWidgetByGlobalCoords(F32x2 pos);
		void RemoveChildByName(const StringID& name);

	protected:
		virtual void RenderImpl(const Rect& destRect);
		virtual void RefreshPlacement();

	private:
		GUIStage& mStage;
		WidgetState mState = WidgetState::WidgetState_Idle;
		bool mIsEnabled = true;
		bool mIsVisible = false;
		bool mIsRoot = false;
		bool mIsIgnoreInputEvent = false;

		Rect mArea;
		LuaRef mScriptHandler;
		std::map<StringID, std::string> mScriptEventHandlers;
	};
	using WidgetPtr = std::shared_ptr<Widget>;
}
// register enum infos
template<>
struct EnumInfoTraits<WidgetType>
{
	static const std::string enumName;
	static const EnumInfo<WidgetType>::EnumType enumInfos;
};
}