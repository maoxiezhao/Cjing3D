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

// test
#include "test\guiTest.h"

namespace Cjing3D
{
	namespace {
		std::unique_ptr<GUITest> mGUITest = nullptr;
	}

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

#ifdef _ENABLE_GAME_EDITOR_
		auto& guiStage = GlobalGetSubSystem<GUIStage>();
		Editor::InitializeEditor(guiStage.GetImGUIStage());
#endif // _ENABLE_GAME_EDITOR_

		// gui test
		mGUITest = std::make_unique<GUITest>();
		mGUITest->DoTest();
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