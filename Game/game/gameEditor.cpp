#include "gameEditor.h"

#include "engine.h"
#include "scripts\luaContext.h"
#include "renderer\renderer.h"
#include "renderer\renderer2D.h"
#include "system\sceneSystem.h"
#include "renderer\paths\renderPath_tiledForward.h"
#include "gui\guiStage.h"
#include "audio\audio.h"

#include "editor\guiEditor.h"
#include "editor\guiEditorScene.h"

#include <thread>
#include <Windows.h>

namespace Cjing3D
{
	GameEditor::GameEditor()
	{
	}

	GameEditor::~GameEditor()
	{
	}

	void GameEditor::Initialize()
	{
		RenderPathTiledForward* path = new RenderPathTiledForward();
		Renderer::SetCurrentRenderPath(path);
		Renderer::GetRenderer2D().SetCurrentRenderPath(path);

#ifdef _ENABLE_GAME_EDITOR_
		auto& guiStage = GlobalGetSubSystem<GUIStage>();
		Editor::InitializeEditor(guiStage.GetImGUIStage());
#endif // _ENABLE_GAME_EDITOR_

		auto& guiRenderer = guiStage.GetGUIRenderer();
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
			Editor::LoadSceneFromOpenFile();
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