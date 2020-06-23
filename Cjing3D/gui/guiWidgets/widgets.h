#pragma once

#include "gui\guiInclude.h"
#include "gui\guiCore\guiDispatcher.h"
#include "scripts\luaRef.h"
#include "renderer\2D\sprite.h"

#include <set>

namespace Cjing3D
{
class GUIStage;
class GUIRenderer;

namespace Gui {

	enum class HierarchySortOrder 
	{
		Front = 0,
		Sortable = 1,
		Back = 2,
	};

	struct GUIScriptEventHandlers
	{
		static const StringID OnLoaded;
		static const StringID OnUnloaded;

		static std::set<StringID> registeredScriptEventHandlers;
	};

	class Widget : public TreeNode<Widget>, public Gui::Dispatcher
	{
	public:
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
		bool CanMouseFocus()const;
		void SetHierarchySortOrder(HierarchySortOrder order) { mHierarchySortOrder = order; }
		HierarchySortOrder GetHierarchySortOrder()const { return mHierarchySortOrder; }
		void SetOrderValue(F32 value) { mOrderValue = value; }
		F32 GetOrderValue()const { return mOrderValue; }

		virtual WidgetType GetSelfWidgetType() const {
			return Widget::GetWidgetType();
		}

		static WidgetType GetWidgetType() {
			return WidgetType::WidgetType_BaseWidget;
		}

		std::shared_ptr<Widget> FindChildByName(const StringID& name);
		std::shared_ptr<Widget> GetChildWidgetByGlobalCoords(F32x2 pos);
		void RemoveChildByName(const StringID& name);

		Widget* GetRoot();

	protected:
		virtual void RenderImpl(const Rect& destRect);
		virtual void RefreshPlacement();

	private:
		GUIStage& mStage;
		bool mIsEnabled = true;
		bool mIsVisible = false;
		bool mIsRoot = false;
		bool mIsIgnoreInputEvent = false;

		Rect mArea;

		LuaRef mScriptHandler;
		std::map<StringID, std::string> mScriptEventHandlers;

		HierarchySortOrder mHierarchySortOrder = HierarchySortOrder::Sortable;
		F32 mOrderValue = 0.0f;	// the small one is in front
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