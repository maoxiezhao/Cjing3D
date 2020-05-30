#include "guiStage.h"
#include "guiRenderer.h"
#include "imguiStage.h"
#include "widgetManager.h"
#include "engine.h"
#include "core\systemContext.hpp"
#include "renderer\renderer.h"
#include "helper\logger.h"

namespace Cjing3D
{
	namespace {

#ifdef CJING_IMGUI_ENABLE
		IMGUIStage mImGuiStage;
#endif
	}

	using namespace Gui;

	GUIStage::GUIStage(SystemContext & systemContext):
		SubSystem(systemContext),
		mRenderer(nullptr)
	{
	}

	GUIStage::~GUIStage()
	{
	}

	void GUIStage::Initialize()
	{
		mRenderer = std::make_unique<GUIRenderer>(*this);

		mWidgetManager = std::make_unique<WidgetManager>(*this);
		mWidgetManager->Initialize();

#ifdef CJING_IMGUI_ENABLE
		mImGuiStage.Initialize();
		mRenderer->SetImGuiStage(&mImGuiStage);
#endif

		SystemContext& systemContext = SystemContext::GetSystemContext();
		Renderer& renderer = systemContext.GetSubSystem<Renderer>();
		U32x2 screenSize = renderer.GetScreenSize();
		
		// initialize root widget
		mRootWidget = std::make_shared<Widget>(*this, STRING_ID(root));
		mRootWidget->SetArea(Rect(
			0.0f, 0.0f,
			(F32)screenSize[0], (F32)screenSize[1]
		));
		mRootWidget->SetVisible(true);

		LoadRegisteredKeys();

		InputManager& inputManager = systemContext.GetSubSystem<InputManager>();
		mPrevMousePos = inputManager.GetMousePos();

		Logger::Info("GUI Stage initialized");
	}

	void GUIStage::Uninitialize()
	{
#ifdef CJING_IMGUI_ENABLE
		mImGuiStage.Uninitialize();
#endif
		mWidgetManager->Uninitialize();
		mWidgetManager.reset();

		mRootWidget.reset();

		Logger::Info("GUI Stage uninitialized");
	}

	void GUIStage::Update(F32 deltaTime)
	{
		PROFILER_BEGIN_CPU_BLOCK("GuiUpdate");

#ifdef CJING_IMGUI_ENABLE
		mImGuiStage.Update(deltaTime);
#endif
		// notify input
		NotifyInput();

		// handle events
		HandleInputEvents();

		// update all widgets
		mRootWidget->Update(deltaTime);

		PROFILER_END_BLOCK();
	}

	WidgetPtr GUIStage::LoadWidgetFromXML(const StringID& name, const std::string& filePath, LuaRef scriptHandler)
	{
		return LoadWidgetFromXML(mRootWidget, name, filePath, scriptHandler);
	}

	WidgetPtr GUIStage::LoadWidgetFromXML(WidgetPtr parent, const StringID& name, const std::string& filePath, LuaRef scriptHandler)
	{
		Logger::Info("GUI Stage Load widget from xml, name:" + name.GetString() + ", path:" + filePath);
		WidgetPtr widget = mWidgetManager->CreateWidgetFromXMLFile(filePath, scriptHandler);
		if (widget == nullptr) {
			Logger::Warning("GUI Stage Load widget from xml failed, name:" + name.GetString() + ", path:" + filePath);
		}

		widget->SetName(name);

		if (parent != nullptr) {
			parent->Add(widget);
		}

		return widget;
	}

	void GUIStage::AddRegisterKeyBoardKey(KeyCode key)
	{
		mRegisteredKeyBoardKeys.push_back(key);
	}

	void GUIStage::SetImGUIStageVisible(bool visible)
	{
		mImGuiStage.SetVisible(visible);
	}

	bool GUIStage::IsImGUIStageVisible() const
	{
		return mImGuiStage.IsVisible();
	}

	IMGUIStage& GUIStage::GetImGUIStage()
	{
		return mImGuiStage;
	}

	void GUIStage::LoadRegisteredKeys()
	{
		mRegisteredMouseKeys.push_back(KeyCode::Click_Left);
		mRegisteredMouseKeys.push_back(KeyCode::Click_Middle);
		mRegisteredMouseKeys.push_back(KeyCode::Click_Right);
	}

	void GUIStage::NotifyInput()
	{
		// 响应输入，并将其以事件的形式分发给widgets
		SystemContext& systemContext = SystemContext::GetSystemContext();
		InputManager& inputManager = systemContext.GetSubSystem<InputManager>();

		// notify mouse event
		for (auto keyCode : mRegisteredMouseKeys)
		{
			if (inputManager.IsKeyDown(keyCode)) {
				GUIInputEvent e = {};
				e.type = GUI_INPUT_EVENT_TYPE_MOUSE_BUTTONDOWN;
				e.key = keyCode;
				mInputEventQueue.push(e);
			}
			else if (inputManager.IsKeyUp(keyCode)) {
				GUIInputEvent e = {};
				e.type = GUI_INPUT_EVENT_TYPE_MOUSE_BUTTONUP;
				e.key = keyCode;
				mInputEventQueue.push(e);
			}
		}

		I32x2 mousePos = inputManager.GetMousePos();
		if (mousePos != mPrevMousePos)
		{
			mPrevMousePos = mousePos;
			GUIInputEvent e = {};
			e.type = GUI_INPUT_EVENT_TYPE_MOUSE_MOTION;
			e.pos = mousePos;
			mInputEventQueue.push(e);
		}

		// notify keyboard event
		for (auto keyCode : mRegisteredKeyBoardKeys)
		{
			if (inputManager.IsKeyDown(keyCode)) {
				GUIInputEvent e = {};
				e.type = GUI_INPUT_EVENT_TYPE_KEYBOARD_KEYDOWN;
				e.key = keyCode;
				mInputEventQueue.push(e);
			}
			else if (inputManager.IsKeyUp(keyCode)) {
				GUIInputEvent e = {};
				e.type = GUI_INPUT_EVENT_TYPE_KEYBOARD_KEYUP;
				e.key = keyCode;
				mInputEventQueue.push(e);
			}
		}

		// notify viewport size changed

	} 

	void GUIStage::HandleInputEvents()
	{
		while (!mInputEventQueue.empty())
		{
			GUIInputEvent e = mInputEventQueue.front();
			mInputEventQueue.pop();

			switch (e.type)
			{
			case GUI_INPUT_EVENT_TYPE_KEYBOARD_KEYDOWN:
				HandldKeyboardButton(e.key, GUI_INPUT_KEY_STATE_KEYDOWN);
				break;
			case GUI_INPUT_EVENT_TYPE_KEYBOARD_KEYUP:
				HandldKeyboardButton(e.key, GUI_INPUT_KEY_STATE_KEYUP);
				break;
			case GUI_INPUT_EVENT_TYPE_MOUSE_MOTION:
				HandleMouseMove(e.pos);
				break;
			case GUI_INPUT_EVENT_TYPE_MOUSE_BUTTONDOWN:
				HandleMouseButton(e.key, GUI_INPUT_KEY_STATE_KEYDOWN);
				break;
			case GUI_INPUT_EVENT_TYPE_MOUSE_BUTTONUP:
				HandleMouseButton(e.key, GUI_INPUT_KEY_STATE_KEYUP);
				break;
			default:
				break;
			}
		}
	}

	void GUIStage::HandldKeyboardButton(KeyCode key, GUI_INPUT_KEY_STATE state)
	{
		if (mCaptureWidget != nullptr) {
			HandleKeyboardButtonEvent(*mRootWidget, mCaptureWidget, key, state);
			return;
		}

		if (mRootWidget == nullptr) {
			return;
		}

		WidgetPtr targetWidget = mMouseFocusWidget;
		if (targetWidget == nullptr) {
			targetWidget = mRootWidget;
		}

		HandleKeyboardButtonEvent(*mRootWidget, targetWidget, key, state);
	}

	void GUIStage::HandleMouseButton(KeyCode key, GUI_INPUT_KEY_STATE state)
	{
		if (mCaptureWidget != nullptr) {
			HandleMouseButtonEvent(*mRootWidget, mCaptureWidget, key, state);
			return;
		}

		if (mRootWidget == nullptr) {
			return;
		}

		WidgetPtr targetWidget = mMouseFocusWidget;
		if (targetWidget == nullptr) {
			targetWidget = mRootWidget;
		}

		HandleMouseButtonEvent(*mRootWidget, targetWidget, key, state);

		mLastClickWidget = targetWidget;
		mLastClickTime;
	}

	void GUIStage::HandleMouseMove(I32x2 mousePos)
	{
		if (mCaptureWidget != nullptr) {
			mCaptureWidget->Fire(UI_EVENT_TYPE::UI_EVENT_MOUSE_MOTION, mMouseFocusWidget.get(), VariantArray());
			return;
		}

		if (mRootWidget == nullptr) {
			return;
		}

		F32x2 fMousePos = { (F32)mousePos[0], (F32)mousePos[1] };
		WidgetPtr targetWidget = mRootWidget->GetChildWidgetByGlobalCoords(fMousePos);
		if (targetWidget != mMouseFocusWidget)
		{
			if (mMouseFocusWidget != nullptr) {
				mRootWidget->Fire(UI_EVENT_TYPE::UI_EVENT_MOUSE_LEAVE, mMouseFocusWidget.get(), VariantArray());
			}

			mMouseFocusWidget = targetWidget;
				
			if (mMouseFocusWidget != nullptr){
				mRootWidget->Fire(UI_EVENT_TYPE::UI_EVENT_MOUSE_ENTER, mMouseFocusWidget.get(), VariantArray());
			}
		}

		if (targetWidget == nullptr) {
			targetWidget = mRootWidget;
		}

		mRootWidget->Fire(UI_EVENT_TYPE::UI_EVENT_MOUSE_MOTION, mMouseFocusWidget.get(), VariantArray());
	}

	void GUIStage::HandleKeyboardButtonEvent(Widget& dispatcher, WidgetPtr targetWidget, KeyCode key, GUI_INPUT_KEY_STATE state)
	{
		if (state == GUI_INPUT_KEY_STATE_KEYDOWN)
		{
			VariantArray variants;
			variants.push_back(Variant(static_cast<U32>(key)));

			dispatcher.Fire(UI_EVENT_TYPE::UI_EVENT_KEYBOARD_KEYDOWN, targetWidget.get(), variants);
		}
		else
		{
			VariantArray variants;
			variants.push_back(Variant(static_cast<U32>(key)));

			dispatcher.Fire(UI_EVENT_TYPE::UI_EVENT_KEYBOARD_KEYUP, targetWidget.get(), variants);
		}
	}

	void GUIStage::HandleMouseButtonEvent(Widget& dispatcher, WidgetPtr targetWidget, KeyCode key, GUI_INPUT_KEY_STATE state)
	{
		if (state == GUI_INPUT_KEY_STATE_KEYDOWN)
		{
			VariantArray variants;
			variants.push_back(Variant(static_cast<U32>(key - KeyCode::Click_Left)));

			dispatcher.Fire(UI_EVENT_TYPE::UI_EVENT_MOUSE_BUTTON_DOWN, targetWidget.get(), variants);
		}
		else
		{
			VariantArray variants;
			variants.push_back(Variant(static_cast<U32>(key - KeyCode::Click_Left)));

			dispatcher.Fire(UI_EVENT_TYPE::UI_EVENT_MOUSE_BUTTON_UP, targetWidget.get(), variants);
		}
	}

	// 仅由GUIRenderer调用 
	void GUIStage::RenderImpl()
	{
		mRootWidget->Render({0.0f, 0.0f});
	}
}