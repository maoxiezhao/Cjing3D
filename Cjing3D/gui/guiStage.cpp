#include "gui\guiStage.h"
#include "gui\guiRenderer.h"
#include "gui\imguiStage.h"
#include "gui\guiCore\guiEvents.h"
#include "renderer\renderer.h"
#include "helper\logger.h"

#include "gui\guiWidgets\widgetManager.h"

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
		mWidgetManager = std::make_unique<Gui::WidgetManager>(*this);
		mWidgetManager->Initialize();

#ifdef CJING_IMGUI_ENABLE
		mImGuiStage.Initialize();
		mRenderer->SetImGuiStage(&mImGuiStage);
#endif
		SystemContext& systemContext = SystemContext::GetSystemContext();
		Renderer& renderer = systemContext.GetSubSystem<Renderer>();
		U32x2 screenSize = renderer.GetScreenSize();

		// initialize widget hierarchy		
		mWidgetHierarchy = std::make_unique<Gui::WidgetHierarchy>(*this);
		mWidgetHierarchy->Initialize(screenSize);

		// load registered keys
		LoadRegisteredKeys();

		// save previous mouse pos
		InputManager& inputManager = systemContext.GetSubSystem<InputManager>();
		mPrevMousePos = inputManager.GetMousePos();

		Logger::Info("GUI Stage initialized");
	}

	void GUIStage::Uninitialize()
	{
#ifdef CJING_IMGUI_ENABLE
		mImGuiStage.Uninitialize();
#endif
		mWidgetHierarchy->Uninitialize();
		mWidgetManager->Uninitialize();

		Logger::Info("GUI Stage uninitialized");
	}

	void GUIStage::Update(F32 deltaTime)
	{
		PROFILER_BEGIN_CPU_BLOCK("GuiUpdate");

#ifdef CJING_IMGUI_ENABLE
		mImGuiStage.Update(deltaTime);
#endif
		mWidgetHierarchy->Update(deltaTime);

		PROFILER_END_BLOCK();
	}

	void GUIStage::FixedUpdate()
	{
		PROFILER_BEGIN_CPU_BLOCK("GuiFixedUpdate");

		// notify input
		NotifyInput();

		// handle events
		HandleInputEvents();

		// update all widgets
		mWidgetHierarchy->FixedUpdate();

		PROFILER_END_BLOCK();
	}

	WidgetPtr GUIStage::LoadWidgetFromXML(const StringID& name, const std::string& filePath, LuaRef scriptHandler)
	{
		return LoadWidgetFromXML(mWidgetHierarchy->GetRootWidget(), name, filePath, scriptHandler);
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
				Gui::GUIInputEvent e = {};
				e.type = GUI_INPUT_EVENT_TYPE_MOUSE_BUTTONDOWN;
				e.key = keyCode;
				mInputEventQueue.push(e);
			}
			else if (inputManager.IsKeyUp(keyCode)) {
				Gui::GUIInputEvent e = {};
				e.type = GUI_INPUT_EVENT_TYPE_MOUSE_BUTTONUP;
				e.key = keyCode;
				mInputEventQueue.push(e);
			}
		}

		I32x2 mousePos = inputManager.GetMousePos();
		if (mousePos != mPrevMousePos)
		{
			mPrevMousePos = mousePos;
			Gui::GUIInputEvent e = {};
			e.type = GUI_INPUT_EVENT_TYPE_MOUSE_MOTION;
			e.pos = mousePos;
			mInputEventQueue.push(e);
		}

		// notify keyboard event
		for (auto keyCode : mRegisteredKeyBoardKeys)
		{
			if (inputManager.IsKeyDown(keyCode)) {
				Gui::GUIInputEvent e = {};
				e.type = GUI_INPUT_EVENT_TYPE_KEYBOARD_KEYDOWN;
				e.key = keyCode;
				mInputEventQueue.push(e);
			}
			else if (inputManager.IsKeyUp(keyCode)) {
				Gui::GUIInputEvent e = {};
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
			Gui::GUIInputEvent e = mInputEventQueue.front();
			mInputEventQueue.pop();

			mWidgetHierarchy->HandleInputEvents(e);
		}
	}

	// 仅由GUIRenderer调用 
	void GUIStage::RenderImpl()
	{
		mWidgetHierarchy->Render();
	}
}