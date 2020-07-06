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

	StringID GUIScriptEventHandlers::GetScriptEventHandlerByUIEventType(UI_EVENT_TYPE eventType)
	{
		StringID ret;
		switch (eventType)
		{
		case UI_EVENT_TYPE::UI_EVENT_MOUSE_ENTER:
			break;
		case UI_EVENT_TYPE::UI_EVENT_MOUSE_LEAVE:
			break;
		case UI_EVENT_TYPE::UI_EVENT_MOUSE_MOTION:
			break;
		case UI_EVENT_TYPE::UI_EVENT_MOUSE_BUTTON_DOWN:
			break;
		case UI_EVENT_TYPE::UI_EVENT_MOUSE_BUTTON_UP:
			break;
		case UI_EVENT_TYPE::UI_EVENT_MOUSE_BUTTON_CLICK:
			break;
		case UI_EVENT_TYPE::UI_EVENT_KEYBOARD_KEYDOWN:
			break;
		case UI_EVENT_TYPE::UI_EVENT_KEYBOARD_KEYUP:
			break;
		default:
			break;
		}

		return ret;
	}

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

		UpdateImpl(dt);

		for (auto& child : mChildren) {
			child->Update(dt);
		}
	}

	void Widget::FixedUpdate()
	{
		if (!IsVisible()) {
			return;
		}

		FixedUpdateImpl();

		for (auto& child : mChildren) {
			child->FixedUpdate();
		}
	}

	void Widget::UpdateAnimation()
	{
	}

	void Widget::Render(const F32x2& offset)
	{
		if (!IsVisible()) {
			return;
		}

		Rect destRect(mArea);
		destRect.Offset(offset);

		if (GetGUIStage().IsDebugDraw())
		{
			if (mDebugSprite.GetColor() == Color4::White()) {
				Color4 randomColor;
				randomColor.SetA(255);
				randomColor.SetR(Random::GetRandomInt(255));
				randomColor.SetG(Random::GetRandomInt(255));
				randomColor.SetB(Random::GetRandomInt(255));
				mDebugSprite.SetColor(randomColor);
			}

			mDebugSprite.SetPos(destRect.GetPos());
			mDebugSprite.SetSize(destRect.GetSize());
			GetGUIRenderer().RenderSprite(mDebugSprite);
		}

		RenderImpl(destRect);

		F32x2 childOffset = destRect.GetPos();
		for (auto& child : mChildren) {
			child->Render(childOffset);
		}
	}

	void Widget::Clear()
	{
		for (auto& child : mChildren)
		{
			if (child != nullptr) {
				child->Clear();
			}
		}

		ClearChildren();
	}

	GUIRenderer& Widget::GetGUIRenderer()
	{
		return mStage.GetGUIRenderer();
	}

	F32x2 Widget::TransformToLocalCoord(const F32x2 point) const
	{
		F32x2 result = point;
		auto parent = GetParent();
		while (parent != nullptr)
		{
			result -= parent->GetArea().GetPos();
			parent = parent->GetParent();
		}
		result -= GetPos();

		return result;
	}

	F32x2 Widget::TransformToGlobalCoord(const F32x2 point) const
	{
		F32x2 result = point;
		auto parent = GetParent();
		while (parent != nullptr)
		{
			result += mParent->GetArea().GetPos();
			parent = parent->GetParent();
		}

		result += GetPos();
		return result;
	}

	// simple version. just check rect.
	bool Widget::HitTest(const F32x2& point) const
	{
		if (mIsVisible == false || mIsIgnoreInputEvent) {
			return false;
		}

		return mArea.Intersects(TransformToLocalCoord(point) + mArea.GetPos());
	}

	void Widget::SetPos(const F32x2 pos)
	{
		mArea.SetPos(pos);
		OnWidgetMoved();
	}

	F32x2 Widget::GetPos() const
	{
		return mArea.GetPos();
	}

	F32x2 Widget::GetGlobalPos() const
	{
		return TransformToGlobalCoord(F32x2());
	}

	void Widget::SetSize(const F32x2 size)
	{
		mArea.SetSize(size);
	}

	F32x2 Widget::GetSize() const
	{
		return mArea.GetSize();
	}

	void Widget::SetFixedSize(F32x2 size)
	{
		mFixedSize = size;
	}

	void Widget::UpdateBestSize()
	{
		F32x2 bestSize = GetBestSize();
		if (bestSize != GetSize())
		{
			SetSize(bestSize);
			OnBestSizeChanged();
		}
	}

	F32x2 Widget::CalculateBestSize() const
	{
		if (!IsVisible()) {
			return F32x2(0.0f, 0.0f);
		}

		return mArea.GetSize();
	}

	void Widget::UpdateLayout()
	{
	}

	void Widget::SetLayoutOffset(F32x2 offset)
	{
		mLayoutOffset = offset;
		OnBestSizeChanged();
	}

	void Widget::SetVerticalAlign(AlignmentMode mode)
	{
		if (mVerticalAlign != mode)
		{
			mVerticalAlign = mode;
			OnAlignModeChanged();
		}
	}

	void Widget::SetHorizontalAlign(AlignmentMode mode)
	{
		if (mHorizontalAlign != mode)
		{
			mHorizontalAlign = mode;
			OnAlignModeChanged();
		}
	}

	void Widget::SetSizeAndFixedSize(F32x2 size)
	{
		SetSize(size);
		SetFixedSize(size);
	}

	F32x2 Widget::GetBestSize() const
	{
		F32x2 fs = GetFixedSize();
		if (fs[0] == 0.0f || fs[1] == 0.0f) 
		{
			F32x2 ps = CalculateBestSize();
			fs = {
				fs[0] ? fs[0] : ps[0],
				fs[1] ? fs[1] : ps[1]
			};
		}
		return fs;
	}

	F32x2 Widget::GetAvailableSize() const
	{
		F32x2 ps = mArea.GetSize();
		F32x2 fs = GetFixedSize();
		return {
			fs[0] ? fs[0] : ps[0],
			fs[1] ? fs[1] : ps[1]
		};
	}

	void Widget::SetWidth(F32 width)
	{
		SetSize({ width, GetHeight() });
	}

	F32 Widget::GetWidth() const
	{
		return mArea.GetWidth();
	}

	void Widget::SetHeight(F32 height)
	{
		SetSize({ GetWidth(), height});
	}

	F32 Widget::GetHeight() const
	{
		return mArea.GetHeight();
	}

	void Widget::SetArea(const Rect& rect)
	{
		mArea = rect;
		OnWidgetMoved();
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

	bool Widget::HaveScriptEventHandler(const StringID& eventName) const
	{
		return mScriptEventHandlers.find(eventName) != mScriptEventHandlers.end();
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

	WidgetHierarchy& Widget::GetWidgetHierarchy()
	{
		return mStage.GetWidgetHierarchy();
	}

	void Widget::RenderImpl(const Rect& destRect)
	{
		F32x2 rectSize = destRect.GetSize();
		if (rectSize[0] <= 0 || rectSize[1] <= 0) {
			return;
		}

		GUIRenderer& renderer = GetGUIRenderer();
	}

	bool Widget::OnWidgetMoved(void)
	{
		return true;
	}

	void Widget::OnFocusd()
	{
	}

	void Widget::OnUnFocusd()
	{
	}

	void Widget::OnTextInput(const UTF8String& text)
	{
	}

	void Widget::OnKeyDown(KeyCode key, int mod)
	{
	}

	void Widget::OnKeyUp(KeyCode key, int mod)
	{
	}

	void Widget::UpdateImpl(F32 dt)
	{
	}

	void Widget::FixedUpdateImpl()
	{
	}

	void Widget::OnLoaded()
	{
		// 在lua中执行初始化，需要传入properties，可能有自定义的属性
		CallScriptEventHandler(GUIScriptEventHandlers::OnLoaded);
	}

	void Widget::OnUnloaded()
	{
		CallScriptEventHandler(GUIScriptEventHandlers::OnUnloaded);
	}
}
}