#pragma once

#include "gui\guiInclude.h"
#include "gui\guiRenderer.h"
#include "gui\widgets.h"
#include "gui\widgetManager.h"
#include "input\InputSystem.h"

#include <queue>

namespace Cjing3D
{
	enum GUI_EVENT_TYPE
	{
		GUI_EVENT_TYPE_UNKNOW,
		GUI_EVENT_TYPE_KEYBOARD_KEYDOWN,
		GUI_EVENT_TYPE_KEYBOARD_KEYUP,
		GUI_EVENT_TYPE_MOUSE_MOTION,
		GUI_EVENT_TYPE_MOUSE_BUTTONDOWN,
		GUI_EVENT_TYPE_MOUSE_BUTTONUP,
	};

	enum GUI_KEY_STATE
	{
		GUI_KEY_STATE_KEYDOWN,
		GUI_KEY_STATE_KEYUP,
	};

	struct GUIEvent
	{
		GUI_EVENT_TYPE type = GUI_EVENT_TYPE_UNKNOW;
		KeyCode key;
		I32x2 pos;
	};

	class GUIStage : public SubSystem
	{
	public:
		friend class GUIRenderer;

		GUIStage(SystemContext& systemContext);
		~GUIStage();

		void Initialize();
		void Uninitialize();
		void Update(F32 deltaTime);

		WidgetPtr GetRootWidget() { return mRootWidget; }
		GUIRenderer& GetGUIRenderer() { return *mRenderer; }
		const GUIRenderer& GetGUIRenderer()const { return *mRenderer; }
		WidgetManager& GetWidgetManager() { return *mWidgetManager; }

	private:
		void LoadRegisteredKeys();
		void NotifyInput();
		void HandleInputEvents();
		void HandldKeyboardButton(KeyCode key, GUI_KEY_STATE state);
		void HandleMouseButton(KeyCode key, GUI_KEY_STATE state);
		void HandleMouseMove(I32x2 mousePos);

		void RenderImpl();

	private:
		// GUI Renderer
		// 在renderPath2D的render阶段会调用GUIRenderer的render
		std::unique_ptr<GUIRenderer> mRenderer;
		std::unique_ptr<WidgetManager> mWidgetManager;

		WidgetPtr mRootWidget = nullptr;

		I32x2 mPrevMousePos;

		std::vector<KeyCode> mRegisteredKeyBoardKeys;
		std::vector<KeyCode> mRegisteredMouseKeys;
		std::queue<GUIEvent> mInputEventQueue;
	};
}