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

	GUIRenderer& Widget::GetGUIRenderer()
	{
		return mStage.GetGUIRenderer();
	}

	F32x2 Widget::TransformToLocalCoord(const F32x2 point) const
	{
		F32x2 result = point;
		if (mParent != nullptr)
		{
			result -= mParent->GetArea().GetPos();
			result = mParent->TransformToLocalCoord(result);
		}

		return result;
	}

	F32x2 Widget::TransformToGlobalCoord(const F32x2 point) const
	{
		F32x2 result = point;
		if (mParent != nullptr)
		{
			result += mParent->GetArea().GetPos();
			result = mParent->TransformToLocalCoord(result);
		}

		return result;
	}

	// simple version. just check rect.
	bool Widget::HitTest(const F32x2& point) const
	{
		if (mIsVisible == false || mIsIgnoreInputEvent) {
			return false;
		}

		return mArea.Intersects(TransformToLocalCoord(point));
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
		return TransformToGlobalCoord(mArea.GetPos());
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

	F32x2 Widget::CalculateBestSize() const
	{
		if (mLayout != nullptr) {
			return mLayout->CalculateBestSize(this);
		}
		return mArea.GetSize();
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

	F32 Widget::GetWidth() const
	{
		return mArea.GetWidth();
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

	void Widget::UpdateLayoutImpl(const Rect& destRect)
	{
	}

	bool Widget::OnWidgetMoved(void)
	{
		if (mParent != nullptr)
		{
			const Rect& parentRect = mParent->GetArea();
			Rect childRect(mArea);
			childRect.Offset(parentRect.GetPos());

			mAlignRect.mLeft   = childRect.mLeft - parentRect.mLeft;
			mAlignRect.mTop    = childRect.mTop  - parentRect.mTop;
			mAlignRect.mRight  = parentRect.mRight  - childRect.mRight;
			mAlignRect.mBottom = parentRect.mBottom - childRect.mBottom;
		}
		return true;
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

	void Widget::UpdateLayout()
	{
		if (!IsVisible()) {
			return;
		}

		if (mLayout != nullptr) {
			mLayout->UpdateLayout(this);
		}
		else
		{
			for (auto& child : mChildren)
			{
				F32x2 fixedSize = child->GetFixedSize();
				if (fixedSize[0] == 0.0f || fixedSize[1] == 0.0f) {
					F32x2 bestSize = child->CalculateBestSize();
					child->SetSize({
						fixedSize[0] != 0.0f ? fixedSize[0] : bestSize[0],
						fixedSize[1] != 0.0f ? fixedSize[1] : bestSize[1]
					});
				}
				else
				{
					child->SetSize({ fixedSize[0], fixedSize[1]});
				}
				child->UpdateLayout();
			}
		}

		UpdateLayoutImpl(GetArea());
	}

	F32x2 Widget::GetLayoutOffset() const
	{
		return F32x2();
	}
}
}