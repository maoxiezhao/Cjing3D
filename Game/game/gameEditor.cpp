#include "gameEditor.h"

#include "engine.h"
#include "scripts\luaContext.h"
#include "system\sceneSystem.h"
#include "renderer\paths\renderPath_tiledForward.h"
#include "gui\guiStage.h"
#include "audio\audio.h"
#include "renderer\renderer.h"
#include "renderer\2D\renderer2D.h"
#include "renderer\2D\font.h"

#include "editor\guiEditor.h"
#include "editor\guiEditorScene.h"

#include <thread>
#include <Windows.h>

#include "utils\string\utf8String.h"
#include "renderer\2D\textDrawable.h"

// test
#include "utils\signal\signal.h"
#include "gui\guiWidgets\stackPanel.h"
#include "gui\guiWidgets\button.h"


namespace Cjing3D
{
	GameEditor::GameEditor()
	{
	}

	GameEditor::~GameEditor()
	{
	}

	TextDrawable textDrawable;

	void GameEditor::Initialize()
	{
		RenderPathTiledForward* path = new RenderPathTiledForward();
		Renderer::SetCurrentRenderPath(path);

		Font::LoadFontTTF("Fonts/arial.ttf");

		auto& guiStage = GlobalGetSubSystem<GUIStage>();
#ifdef _ENABLE_GAME_EDITOR_
		Editor::InitializeEditor(guiStage.GetImGUIStage());
#endif // _ENABLE_GAME_EDITOR_

		Gui::WidgetHierarchy& widgetHierarchy = guiStage.GetWidgetHierarchy();
		// Test GUI
		Gui::WidgetPtr stack = std::make_shared<Gui::StackPanel>(guiStage, StringID("test"), 200.0f, 200.0f);
		stack->SetPos({ 500.0f, 200.0f });
		stack->SetVisible(true);
		widgetHierarchy.AddWidget(stack);

		std::shared_ptr<Gui::Button> button = std::make_shared<Gui::Button>(guiStage, StringID("button"));
		button->SetPos({ 10.0f, 10.0f });
		button->SetText("Test");
		button->SetVisible(true);
		stack->Add(button);
		mConnections(button->mClickCallback, [button]() {
			Logger::Info("CLICK!!!!");
			button->SetText(u8"Test2");
		});
	}

	void GameEditor::Update(EngineTime time)
	{
	}

	void GameEditor::FixedUpdate()
	{

#ifdef _ENABLE_GAME_EDITOR_
		auto& inputManager = GlobalGetSubSystem<InputManager>();
		if (inputManager.IsKeyDown(KeyCode::Esc)) {
			GlobalGetEngine()->SetIsExiting(true);
		}

		if (inputManager.IsKeyDown(KeyCode::F4))
		{
			auto& guiStage = GlobalGetSubSystem<GUIStage>();
			bool show = guiStage.IsImGUIStageVisible();
			guiStage.SetImGUIStageVisible(!show);
		}

		// test scene serializing
		if (inputManager.IsKeyDown(KeyCode::F5)) 
		{
			//Editor::LoadSceneFromOpenFile();
		}
		else if (inputManager.IsKeyDown(KeyCode::F6)) 
		{
			Editor::SaveSceneToOpenFile();
		}
		else if (inputManager.IsKeyDown(KeyCode::F7))
		{
			Scene::GetScene().Clear();
		}
#endif // _ENABLE_GAME_EDITOR_

	}

	void GameEditor::Uninitialize()
	{
	}
}