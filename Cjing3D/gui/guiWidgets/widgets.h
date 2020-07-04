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
	class WidgetHierarchy;

	enum AlignmentMode
	{
		AlignmentMode_Begin = 0,
		AlignmentMode_Center,
		AlignmentMode_End,
		AlignmentMode_Fill,
	};
	enum AlignmentOrien
	{
		AlignmentOrien_Horizontal = 0,
		AlignmentOrien_Vertical
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

		WidgetMargin() {}
		WidgetMargin(F32 margin) : left(margin), top(margin), right(margin), bottom(margin) {}
		WidgetMargin(F32 left, F32 top, F32 right, F32 bottom) :
			left(left), top(top), right(right), bottom(bottom) {}
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
		virtual void Clear();

		GUIRenderer& GetGUIRenderer();
		GUIStage& GetGUIStage();
		WidgetHierarchy& GetWidgetHierarchy();

		bool HitTest(const F32x2& point)const;
		F32x2 TransformToLocalCoord(const F32x2 point)const;
		F32x2 TransformToGlobalCoord(const F32x2 point)const;

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
		F32x2 GetGlobalPos()const;
		void SetSize(const F32x2 size);
		F32x2 GetSize()const;
		void SetFixedSize(F32x2 size);
		F32x2 GetFixedSize()const { return mFixedSize; }
		void SetSizeAndFixedSize(F32x2 size);
		F32x2 GetBestSize()const;
		F32x2 GetAvailableSize()const;
		void SetWidth(F32 width);
		F32 GetWidth()const;
		void SetHeight(F32 height);
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
		virtual void SetNeedLayout(bool needLayout);
		virtual F32x2 GetLayoutOffset()const;
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

		// special event, 这些事件没有冒泡特性，暂时不通过Signal实现
		virtual void OnFocusd();
		virtual void OnUnFocusd();
		virtual void OnTextInput(const UTF8String& text);
		virtual void OnKeyDown(KeyCode key, int mod);
		virtual void OnKeyUp(KeyCode key, int mod);

	protected:
		virtual void UpdateImpl(F32 dt);
		virtual void FixedUpdateImpl();
		virtual void RenderImpl(const Rect& destRect);
		virtual void UpdateLayoutImpl(const Rect& destRect);

		virtual void OnParentChanged(Widget* old_parent) {}
		virtual void OnChildAdded(std::shared_ptr<Widget>& node) {}
		virtual void OnChildRemoved(std::shared_ptr<Widget>& node) {}
		virtual bool OnWidgetMoved(void);

	protected:
		// base status
		GUIStage& mStage;
		bool mIsEnabled = true;
		bool mIsVisible = true;
		bool mIsRoot = false;
		bool mIsIgnoreInputEvent = false;
		bool mIsFocused = false;
		Rect mArea; 
		F32x2 mFixedSize;
		HierarchySortOrder mHierarchySortOrder = HierarchySortOrder::Sortable;
		F32 mOrderValue = 0.0f;
		Sprite mDebugSprite;

		// script
		LuaRef mScriptHandler;
		std::map<StringID, std::string> mScriptEventHandlers;

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