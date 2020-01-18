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
#ifdef CJING_IMGUI_ENABLE
		mImGuiStage.Update(deltaTime);
#endif
		// notify input
		NotifyInput();

		// update all widgets
		mRootWidget->Update(deltaTime);
	}

	WidgetPtr GUIStage::LoadWidgetFromXML(const std::string& name)
	{
		Logger::Info("GUI Stage Load widget from xml:" + name);
		WidgetPtr widget = mWidgetManager->CreateWidgetFromXMLFile(name);
		if (widget == nullptr) {
			Logger::Warning("GUI Stage Load widget from xml failed:" + name);
		}

		return widget;
	}

	WidgetPtr GUIStage::LoadWidgetFromXML(Widget& parent, const std::string& name)
	{
		WidgetPtr widget = LoadWidgetFromXML(name);
		if (widget == nullptr) {
			return nullptr;
		}

		parent.Add(widget);
	
		return widget;
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
				GUIEvent e = {};
				e.type = GUI_EVENT_TYPE_MOUSE_BUTTONDOWN;
				e.key = keyCode;
				mInputEventQueue.push(e);
			}
			else if (inputManager.IsKeyUp(keyCode)) {
				GUIEvent e = {};
				e.type = GUI_EVENT_TYPE_MOUSE_BUTTONUP;
				e.key = keyCode;
				mInputEventQueue.push(e);
			}
		}

		I32x2 mousePos = inputManager.GetMousePos();
		if (mousePos != mPrevMousePos)
		{
			mPrevMousePos = mousePos;
			GUIEvent e = {};
			e.type = GUI_EVENT_TYPE_MOUSE_MOTION;
			e.pos = mousePos;
			mInputEventQueue.push(e);
		}

		// notify keyboard event
		for (auto keyCode : mRegisteredKeyBoardKeys)
		{
			if (inputManager.IsKeyDown(keyCode)) {
				GUIEvent e = {};
				e.type = GUI_EVENT_TYPE_KEYBOARD_KEYDOWN;
				e.key = keyCode;
				mInputEventQueue.push(e);
			}
			else if (inputManager.IsKeyUp(keyCode)) {
				GUIEvent e = {};
				e.type = GUI_EVENT_TYPE_KEYBOARD_KEYUP;
				e.key = keyCode;
				mInputEventQueue.push(e);
			}
		}
	} 

	void GUIStage::HandleInputEvents()
	{
		while (!mInputEventQueue.empty())
		{
			GUIEvent e = mInputEventQueue.front();
			mInputEventQueue.pop();

			switch (e.type)
			{
			case GUI_EVENT_TYPE_KEYBOARD_KEYDOWN:
				HandldKeyboardButton(e.key, GUI_KEY_STATE_KEYDOWN);
				break;
			case GUI_EVENT_TYPE_KEYBOARD_KEYUP:
				HandldKeyboardButton(e.key, GUI_KEY_STATE_KEYUP);
				break;
			case GUI_EVENT_TYPE_MOUSE_MOTION:
				HandleMouseMove(e.pos);
				break;
			case GUI_EVENT_TYPE_MOUSE_BUTTONDOWN:
				HandleMouseButton(e.key, GUI_KEY_STATE_KEYDOWN);
				break;
			case GUI_EVENT_TYPE_MOUSE_BUTTONUP:
				HandleMouseButton(e.key, GUI_KEY_STATE_KEYUP);
				break;
			default:
				break;
			}
		}
	}

	void GUIStage::HandldKeyboardButton(KeyCode key, GUI_KEY_STATE state)
	{
	}

	void GUIStage::HandleMouseButton(KeyCode key, GUI_KEY_STATE state)
	{
	}

	void GUIStage::HandleMouseMove(I32x2 mousePos)
	{
	}

	// 仅由GUIRenderer调用
	void GUIStage::RenderImpl()
	{
		mRootWidget->Render({0.0f, 0.0f});
	}
}