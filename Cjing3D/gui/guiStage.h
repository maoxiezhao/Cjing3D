#pragma once

#include "gui\guiInclude.h"
#include "gui\guiRenderer.h"
#include "gui\widgets.h"
#include "input\InputSystem.h"

#include <queue>

namespace Cjing3D
{
	class WidgetManager;
	class IMGUIStage;

	enum GUI_INPUT_EVENT_TYPE
	{
		GUI_INPUT_EVENT_TYPE_UNKNOW,
		GUI_INPUT_EVENT_TYPE_KEYBOARD_KEYDOWN,
		GUI_INPUT_EVENT_TYPE_KEYBOARD_KEYUP,
		GUI_INPUT_EVENT_TYPE_MOUSE_MOTION,
		GUI_INPUT_EVENT_TYPE_MOUSE_BUTTONDOWN,
		GUI_INPUT_EVENT_TYPE_MOUSE_BUTTONUP,
	};

	enum GUI_INPUT_KEY_STATE
	{
		GUI_INPUT_KEY_STATE_KEYDOWN,
		GUI_INPUT_KEY_STATE_KEYUP,
	};

	struct GUIInputEvent
	{
		GUI_INPUT_EVENT_TYPE type = GUI_INPUT_EVENT_TYPE_UNKNOW;
		KeyCode key;
		I32x2 pos;
	};

	LUA_BINDER_REGISTER_CLASS
	class GUIStage : public SubSystem
	{
	public:
		friend class GUIRenderer;

		LUA_BINDER_REGISTER_CLASS_CONSTRUCTOR
		GUIStage(SystemContext& systemContext);
		~GUIStage();

		void Initialize();
		void Uninitialize();
		void Update(F32 deltaTime);

		WidgetPtr GetRootWidget() { return mRootWidget; }
		GUIRenderer& GetGUIRenderer() { return *mRenderer; }
		const GUIRenderer& GetGUIRenderer()const { return *mRenderer; }
		WidgetManager& GetWidgetManager() { return *mWidgetManager; }

		WidgetPtr LoadWidgetFromXML(const StringID& name, const std::string& filePath, LuaRef scriptHandler = LuaRef::NULL_REF);
		WidgetPtr LoadWidgetFromXML(WidgetPtr parent, const StringID& name, const std::string& filePath, LuaRef scriptHandler = LuaRef::NULL_REF);

		void AddRegisterKeyBoardKey(KeyCode key);

		// imgui
		void SetImGUIStageVisible(bool visible);
		bool IsImGUIStageVisible()const;
		IMGUIStage& GetImGUIStage();

	private:
		void LoadRegisteredKeys();
		void NotifyInput();
		void HandleInputEvents();

		void HandldKeyboardButton(KeyCode key, GUI_INPUT_KEY_STATE state);
		void HandleMouseButton(KeyCode key, GUI_INPUT_KEY_STATE state);
		void HandleMouseMove(I32x2 mousePos);

		void HandleKeyboardButtonEvent(Widget& dispatcher, WidgetPtr targetWidget, KeyCode key, GUI_INPUT_KEY_STATE state);
		void HandleMouseButtonEvent(Widget& dispatcher, WidgetPtr targetWidget, KeyCode key, GUI_INPUT_KEY_STATE state);

		void RenderImpl();

	private:
		// GUI Renderer
		// 在renderPath2D的render阶段会调用GUIRenderer的render
		std::unique_ptr<GUIRenderer> mRenderer;
		std::unique_ptr<WidgetManager> mWidgetManager;

		WidgetPtr mRootWidget = nullptr;
		WidgetPtr mCaptureWidget = nullptr;	// 强制锁定的widget
		WidgetPtr mMouseFocusWidget = nullptr;      // 当前鼠标悬停的widget

		WidgetPtr mLastClickWidget = nullptr;
		U32 mLastClickTime = 0;

		I32x2 mPrevMousePos;

		std::vector<KeyCode> mRegisteredKeyBoardKeys;
		std::vector<KeyCode> mRegisteredMouseKeys;
		std::queue<GUIInputEvent> mInputEventQueue;
	};
}