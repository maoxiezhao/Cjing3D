#include "gui\guiStage.h"
#include "gui\guiRenderer.h"
#include "renderer\renderer.h"
#include "helper\logger.h"

#include "gui\guiWidgets\widgetManager.h"

namespace Cjing3D
{
	using namespace Gui;

	GUIStage::GUIStage()
	{
	}

	GUIStage::~GUIStage()
	{
	}

	void GUIStage::Initialize()
	{
		// initialize gui renderer
		mRenderer = std::make_unique<GUIRenderer>(*this);

		// initialize widget manager
		mWidgetManager = std::make_unique<Gui::WidgetManager>(*this);
		mWidgetManager->Initialize();

		// initialize widget hierarchy		
		mWidgetHierarchy = std::make_unique<Gui::WidgetHierarchy>(*this);
		mWidgetHierarchy->Initialize(Renderer::GetScreenSize());

		// load registered keys
		LoadRegisteredKeys();

		// save previous mouse pos
		mPrevMousePos = GetGlobalContext().gInputManager->GetMousePos();

		Logger::Info("GUI Stage initialized");
	}

	void GUIStage::Uninitialize()
	{
		mWidgetHierarchy->Uninitialize();
		mWidgetManager->Uninitialize();

		Logger::Info("GUI Stage uninitialized");
	}

	void GUIStage::Update(F32 deltaTime)
	{
		if (!IsGUIVisible()) {
			return;
		}

		PROFILER_BEGIN_CPU_BLOCK("GuiUpdate");
		mWidgetHierarchy->Update(deltaTime);
		PROFILER_END_BLOCK();
	}

	void GUIStage::FixedUpdate()
	{
		if (!IsGUIVisible()) {
			return;
		}

		PROFILER_BEGIN_CPU_BLOCK("GuiFixedUpdate");
		// notify input
		NotifyInput();

		// handle events
		HandleInputEvents();

		// update all widgets
		mWidgetHierarchy->FixedUpdate();

		// update all animations
		mWidgetHierarchy->UpdateAnimation();

		PROFILER_END_BLOCK();
	}

	WidgetPtr GUIStage::LoadWidgetFromXML(const StringID& name, const std::string& filePath, LuaRef scriptHandler)
	{
		return LoadWidgetFromXML(nullptr, name, filePath, scriptHandler);
	}

	WidgetPtr GUIStage::LoadWidgetFromXML(WidgetPtr parent, const StringID& name, const std::string& filePath, LuaRef scriptHandler)
	{
		Logger::Info("GUI Stage Load widget from xml, name:" + name.GetString() + ", path:" + filePath);
		WidgetPtr widget = mWidgetManager->CreateWidgetFromXMLFile(filePath, scriptHandler);
		if (widget == nullptr) {
			Logger::Warning("GUI Stage Load widget from xml failed, name:" + name.GetString() + ", path:" + filePath);
		}

		widget->SetName(name);

		if (parent != nullptr) 
		{
			parent->Add(widget);
		}
		else
		{
			mWidgetHierarchy->AddWidget(widget);
		}

		return widget;
	}

	void GUIStage::AddRegisterKeyBoardKey(KeyCode key)
	{
		mRegisteredKeyBoardKeys.push_back(key);
	}


	void GUIStage::PushInputEvent(const GUIInputEvent& ent)
	{
		mInputEventQueue.push(ent);
	}

	void GUIStage::LoadRegisteredKeys()
	{
		mRegisteredMouseKeys.push_back(KeyCode::Click_Left);
		mRegisteredMouseKeys.push_back(KeyCode::Click_Middle);
		mRegisteredMouseKeys.push_back(KeyCode::Click_Right);

		mRegisteredKeyBoardKeys.push_back(KeyCode::Arrow_Left);
		mRegisteredKeyBoardKeys.push_back(KeyCode::Arrow_Right);
		mRegisteredKeyBoardKeys.push_back(KeyCode::Arrow_Down);
		mRegisteredKeyBoardKeys.push_back(KeyCode::Arrow_Up);
	}

	void GUIStage::NotifyInput()
	{
		// 响应输入，并将其以事件的形式分发给widgets
		auto inputManager = GetGlobalContext().gInputManager;

		// notify mouse event
		I32x2 mousePos = inputManager->GetMousePos();
		for (auto keyCode : mRegisteredMouseKeys)
		{
			if (inputManager->IsKeyPressed(keyCode)) {
				Gui::GUIInputEvent e = {};
				e.type = GUI_INPUT_EVENT_TYPE_MOUSE_BUTTONDOWN;
				e.key = keyCode;
				e.pos = mousePos;
				mInputEventQueue.push(e);
			}
			else if (inputManager->IsKeyReleased(keyCode)) {
				Gui::GUIInputEvent e = {};
				e.type = GUI_INPUT_EVENT_TYPE_MOUSE_BUTTONUP;
				e.key = keyCode;
				e.pos = mousePos;
				mInputEventQueue.push(e);
			}
		}

		if (mousePos != mPrevMousePos)
		{
			mPrevMousePos = mousePos;
			Gui::GUIInputEvent e = {};
			e.type = GUI_INPUT_EVENT_TYPE_MOUSE_MOTION;
			e.pos = mousePos;
			mInputEventQueue.push(e);
		}

		auto mouseWheelDelta = inputManager->GetMouseWheelDelta();
		if (mouseWheelDelta != 0)
		{
			Gui::GUIInputEvent e = {};
			e.type = GUI_INPUT_EVENT_TYPE_MOUSE_WHEEL_CHANGED;
			e.delta = (I32)mouseWheelDelta;
			e.pos = mousePos;
			mInputEventQueue.push(e);
		}

		// notify keyboard event
		for (auto keyCode : mRegisteredKeyBoardKeys)
		{
			if (inputManager->IsKeyPressed(keyCode))
			{
				Gui::GUIInputEvent e = {};
				e.type = GUI_INPUT_EVENT_TYPE_KEYBOARD_KEYDOWN;
				e.key = keyCode;

				if (inputManager->IsKeyHold(KeyCode::Shift_Left) || inputManager->IsKeyHold(KeyCode::Shift_Right)) {
					e.modifiers |= GUI_INPUT_KEYBOARD_MODIFIER::mod_shift;
				}
				if (inputManager->IsKeyHold(KeyCode::Alt_Left) || inputManager->IsKeyHold(KeyCode::Alt_Right)) {
					e.modifiers |= GUI_INPUT_KEYBOARD_MODIFIER::mod_alt;
				}
				if (inputManager->IsKeyHold(KeyCode::Ctrl_Left) || inputManager->IsKeyHold(KeyCode::Ctrl_Right)) {
					e.modifiers |= GUI_INPUT_KEYBOARD_MODIFIER::mod_control;
				}

				mInputEventQueue.push(e);
			}
			else if (inputManager->IsKeyReleased(keyCode))
			{
				Gui::GUIInputEvent e = {};
				e.type = GUI_INPUT_EVENT_TYPE_KEYBOARD_KEYUP;
				e.key = keyCode;

				if (inputManager->IsKeyHold(KeyCode::Shift_Left) || inputManager->IsKeyHold(KeyCode::Shift_Right)) {
					e.modifiers |= GUI_INPUT_KEYBOARD_MODIFIER::mod_shift;
				}
				if (inputManager->IsKeyHold(KeyCode::Alt_Left) || inputManager->IsKeyHold(KeyCode::Alt_Right)) {
					e.modifiers |= GUI_INPUT_KEYBOARD_MODIFIER::mod_alt;
				}
				if (inputManager->IsKeyHold(KeyCode::Ctrl_Left) || inputManager->IsKeyHold(KeyCode::Ctrl_Right)) {
					e.modifiers |= GUI_INPUT_KEYBOARD_MODIFIER::mod_control;
				}

				mInputEventQueue.push(e);
			}
		}

		// notify viewport size changed

	} 

	void GUIStage::HandleInputEvents()
	{
		while (!mInputEventQueue.empty())
		{
			Gui::GUIInputEvent e = mInputEventQueue.front();
			mInputEventQueue.pop();
			mWidgetHierarchy->HandleInputEvents(e);
		}
	}

	// 仅由GUIRenderer调用 
	void GUIStage::RenderImpl()
	{
		if (!IsGUIVisible()) {
			return;
		}

		PROFILER_BEGIN_CPU_BLOCK("GuiRender");
		mWidgetHierarchy->Render();
		PROFILER_END_BLOCK();
	}
}