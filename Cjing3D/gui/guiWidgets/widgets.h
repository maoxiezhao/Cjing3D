#pragma once

#include "gui\guiInclude.h"
#include "gui\guiCore\guiDispatcher.h"
#include "scripts\luaRef.h"
#include "renderer\2D\sprite.h"
#include "renderer\2D\textDrawable.h"

#include <set>

namespace Cjing3D
{
class GUIStage;
class GUIRenderer;

namespace Gui {
	class Layout;

	enum WidgetAlignment
	{
		None = 0,
		Left = 1 << 0,
		Top = 1 << 1,
		Right = 1 << 2,
		Bottom = 1 << 3,
		HCenter = 1 << 4,
		VCenter = 1 << 5,
		Horizontal = Left | HCenter | Right,
		Vertical = Top | VCenter | Bottom
	};

	enum class HierarchySortOrder 
	{
		Front = 0,
		Sortable = 1,
		Back = 2,
	};

	struct WidgetMargin final {
		F32 left = 0.0f;
		F32 top = 0.0f;
		F32 right = 0.0f;
		F32 bottom = 0.0f;
	};

	struct GUIScriptEventHandlers
	{
		static const StringID OnLoaded;
		static const StringID OnUnloaded;

		static std::set<StringID> registeredScriptEventHandlers;
		static StringID GetScriptEventHandlerByUIEventType(UI_EVENT_TYPE eventType);
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
		bool HaveScriptEventHandler(const StringID& eventName)const;
		void AddScriptEventHandler(const StringID& eventName, const std::string& handler);
		void CallScriptEventHandler(const StringID& eventName);
		void CallScriptEventHandlerWithVariants(const StringID& eventName, VariantArray variants);

		// basic status
		Widget* GetRoot();
		void SetPos(const F32x2 pos);
		F32x2 GetPos()const;
		void SetSize(const F32x2 size);
		F32x2 GetSize()const;
		void SetFixedSize(F32x2 size);
		F32x2 GetFixedSize()const { return mFixedSize; }
		F32x2 GetBestSize()const;
		F32x2 GetAvailableSize()const;
		F32 GetWidth()const;
		F32 GetHeight()const;
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
		bool IsFocused()const { return mIsFocused; }
		void SetFocused(bool isFocused) { mIsFocused = isFocused; }

		// layout 
		virtual F32x2 CalculateBestSize()const;
		virtual void UpdateLayout();
		void UpdateAlignment(U32 alignMask);
		std::shared_ptr<Layout> GetLayout() { return mLayout; }
		void SetLayout(const std::shared_ptr<Layout>& layout) { mLayout = layout; }

		// widget type
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
		virtual void UpdateLayoutImpl(const Rect& destRect);
		virtual void OnParentChanged(Widget* old_parent) {}
		virtual void OnChildAdded(std::shared_ptr<Widget>& node) {}
		virtual void OnChildRemoved(std::shared_ptr<Widget>& node) {}
		virtual bool onWidgetMoved(void);
		virtual void UpdateImpl(F32 dt);
		virtual void FixedUpdateImpl();

	private:
		GUIStage& mStage;
		bool mIsEnabled = true;
		bool mIsVisible = true;
		bool mIsRoot = false;
		bool mIsIgnoreInputEvent = false;
		bool mIsFocused = false;
		Rect mArea; 
		F32x2 mFixedSize;
		Rect mAlignRect;
		U32 mAlignment = WidgetAlignment::None;
		HierarchySortOrder mHierarchySortOrder = HierarchySortOrder::Sortable;
		F32 mOrderValue = 0.0f;	// the small one is in front

		LuaRef mScriptHandler;
		std::map<StringID, std::string> mScriptEventHandlers;
		Sprite mDebugSprite;

		// layout
		std::shared_ptr<Layout> mLayout = nullptr;
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