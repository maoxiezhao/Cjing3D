#include "gameEditor.h"

#include "engine.h"
#include "scripts\luaContext.h"
#include "renderer\renderer.h"
#include "renderer\renderer2D.h"
#include "system\sceneSystem.h"
#include "renderer\paths\renderPath_forward.h"
#include "gui\guiStage.h"

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
		auto& renderer = GlobalGetSubSystem<Renderer>();
		RenderPathForward* path = new RenderPathForward(renderer);
		renderer.SetCurrentRenderPath(path);
		renderer.GetRenderer2D().SetCurrentRenderPath(path);

#ifdef _ENABLE_GAME_EDITOR_
		auto& guiStage = GlobalGetSubSystem<GUIStage>();
		Editor::InitializeEditor(guiStage.GetImGUIStage());
#endif // _ENABLE_GAME_EDITOR_
	}

	void GameEditor::Update(EngineTime time)
	{
		auto systemContext = GetGameContext();
		auto& inputManager = systemContext->GetSubSystem<InputManager>();
		auto& renderer = systemContext->GetSubSystem<Renderer>();

		if (inputManager.IsKeyDown(KeyCode::Esc)) {
			systemContext->GetEngine()->SetIsExiting(true);
		}

		if (inputManager.IsKeyDown(KeyCode::F4))
		{
			auto& guiStage = systemContext->GetSubSystem<GUIStage>();
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
			renderer.GetMainScene().Clear();
		}
	}

	void GameEditor::Uninitialize()
	{
	}
}