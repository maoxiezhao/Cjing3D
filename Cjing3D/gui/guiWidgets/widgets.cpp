#include "widgets.h"
#include "widgetInclude.h"

namespace Cjing3D
{
const std::string EnumInfoTraits<WidgetType>::enumName = "widget_type";
const EnumInfo<WidgetType>::EnumType EnumInfoTraits<WidgetType>::enumInfos =
{
	{WidgetType::WidgetType_BaseWidget, "BaseWidget"},
	{WidgetType::WidgetType_Panel, "Panel"},
};

namespace Gui {
	const StringID GUIScriptEventHandlers::OnLoaded = STRING_ID(OnLoaded);
	const StringID GUIScriptEventHandlers::OnUnloaded = STRING_ID(OnUnloaded);

	std::set<StringID> GUIScriptEventHandlers::registeredScriptEventHandlers = {
		GUIScriptEventHandlers::OnLoaded,
		GUIScriptEventHandlers::OnUnloaded
	};

	Widget::Widget(GUIStage& stage, const StringID& name) :
		TreeNode<Widget>(name),
		Dispatcher(),
		mStage(stage)
	{
	}

	Widget::~Widget()
	{
	}

	void Widget::Update(F32 dt)
	{
		if (!IsVisible()) {
			return;
		}
	}

	void Widget::FixedUpdate()
	{
		if (!IsVisible()) {
			return;
		}
	}

	void Widget::Render(const F32x2& offset)
	{
		if (!IsVisible()) {
			return;
		}

		Rect destRect(mArea);
		destRect.Offset(offset);

		RenderImpl(destRect);

		F32x2 childOffset = destRect.GetPos();
		for (auto& child : mChildren) {
			child->Render(childOffset);
		}
	}

	GUIRenderer& Widget::GetGUIRenderer()
	{
		return mStage.GetGUIRenderer();
	}

	F32x2 Widget::TransfromToLocalCoord(const F32x2 point) const
	{
		F32x2 result = point;
		if (mParent != nullptr)
		{
			result -= mParent->GetArea().GetPos();
			result = mParent->TransfromToLocalCoord(result);
		}

		return result;
	}

	// simple version. just check rect.
	bool Widget::HitTest(const F32x2& point) const
	{
		if (mIsVisible == false || mIsIgnoreInputEvent) {
			return false;
		}

		return mArea.Intersects(TransfromToLocalCoord(point));
	}

	void Widget::SetPos(const F32x2 pos)
	{
		mArea.SetPos(pos);
	}

	F32x2 Widget::GetPos() const
	{
		return mArea.GetPos();
	}

	void Widget::SetSize(const F32x2 size)
	{
		mArea.SetSize(size);
	}

	F32x2 Widget::GetSize() const
	{
		return mArea.GetSize();
	}

	void Widget::SetArea(const Rect& rect)
	{
		mArea = rect;
	}

	bool Widget::CanMouseFocus() const
	{
		return true;
	}

	std::shared_ptr<Widget> Widget::FindChildByName(const StringID& name)
	{
		return Find(name)->GetNodePtr();
	}

	WidgetPtr Widget::GetChildWidgetByGlobalCoords(F32x2 pos)
	{
		WidgetPtr result = nullptr;
		for (auto& child : mChildren)
		{
			if (child->HitTest(pos) == false) {
				continue;
			}

			result = child->GetChildWidgetByGlobalCoords(pos);
			if (result != nullptr) {
				break;
			}
		}

		if (result == nullptr) {
			result = GetNodePtr();
		}

		return result;
	}

	void Widget::RemoveChildByName(const StringID& name)
	{
		Remove(name);
	}

	Widget* Widget::GetRoot()
	{
		Widget* root = this;
		while (root->GetParent() != nullptr)
		{
			root = root->GetParent();
		}
		return root;
	}

	LuaRef Widget::GetScriptHandler()
	{
		if (mScriptHandler != LuaRef::NULL_REF) {
			return mScriptHandler;
		}

		return mParent != nullptr ? mParent->GetScriptHandler() : LuaRef::NULL_REF;
	}

	void Widget::SetScriptHandler(LuaRef handler)
	{
		// strong ref?
		mScriptHandler = handler;
	}

	void Widget::AddScriptEventHandler(const StringID& eventName, const std::string& handler)
	{
		if (eventName == StringID::EMPTY || handler.empty()) {
			return;
		}

		mScriptEventHandlers[eventName] = handler;
	}

	void Widget::CallScriptEventHandlerWithVariants(const StringID& eventName, VariantArray variants)
	{
		LuaRef scriptHandler = GetScriptHandler();
		if (scriptHandler.IsRefEmpty()) {
			return;
		}

		auto it = mScriptEventHandlers.find(eventName);
		if (it == mScriptEventHandlers.end()) {
			return;
		}

		std::string eventHandler = it->second;
		if (eventHandler.empty()) {
			return;
		}

		LuaApi::BindLuaVariantArray luaVariantArray;
		for (auto& variant : variants) {
			luaVariantArray.EmplaceBack(variant);
		}

		lua_State* l = scriptHandler.GetLuaState();
		scriptHandler.Push();
		LuaTools::Push<Widget>(l, *this);
		LuaTools::Push<LuaApi::BindLuaVariantArray>(l, luaVariantArray);

		LuaContext::DoLuaString(l, eventHandler, 2, 0);
	}

	void Widget::CallScriptEventHandler(const StringID& eventName)
	{
		LuaRef scriptHandler = GetScriptHandler();
		if (scriptHandler.IsRefEmpty()) {
			return;
		}

		auto it = mScriptEventHandlers.find(eventName);
		if (it == mScriptEventHandlers.end()) {
			return;
		}

		std::string eventHandler = it->second;
		if (eventHandler.empty()) {
			return;
		}

		lua_State* l = scriptHandler.GetLuaState();
		scriptHandler.Push();
		LuaTools::Push<Widget>(l, *this);
		LuaContext::DoLuaString(l, eventHandler, 2, 0);
	}

	GUIStage& Widget::GetGUIStage()
	{
		return mStage;
	}

	void Widget::RenderImpl(const Rect& destRect)
	{
		F32x2 rectSize = destRect.GetSize();
		if (rectSize[0] <= 0 || rectSize[1] <= 0) {
			return;
		}

		GUIRenderer& renderer = GetGUIRenderer();
	}

	void Widget::OnLoaded()
	{
		// 在lua中执行初始化，需要传入properties，可能有自定义的属性
		CallScriptEventHandler(GUIScriptEventHandlers::OnLoaded);

		// 重新根据位置和大小刷新布局
		RefreshPlacement();
	}

	void Widget::OnUnloaded()
	{
		CallScriptEventHandler(GUIScriptEventHandlers::OnUnloaded);
	}

	void Widget::RefreshPlacement()
	{
		// 暂时不处理mask
		Rect rect = GetArea();
		for (auto& child : mChildren) {
			child->RefreshPlacement();

			rect.Union(child->GetArea());
		}

		mArea = rect;
	}
}
}