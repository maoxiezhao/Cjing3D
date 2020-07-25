#include "game.h"
#include "engine.h"
#include "renderer\paths\renderPath_tiledForward.h"
#include "gui\guiStage.h"
#include "editor\guiEditor.h"
#include "system\sceneSystem.h"

using namespace Cjing3D;

namespace CjingGame
{
	MainGame::MainGame()
	{
	}

	MainGame::~MainGame()
	{
	}

	void MainGame::Initialize()
	{
		// rendering config
		RenderPathTiledForward* path = new RenderPathTiledForward();
		Renderer::SetCurrentRenderPath(path);
		Font::LoadFontTTF("Fonts/arial.ttf");
		path->SetFXAAEnable(true);

#ifdef _ENABLE_GAME_EDITOR_
		auto& guiStage = GlobalGetSubSystem<GUIStage>();
		Editor::InitializeEditor(guiStage.GetImGUIStage());
#endif // _ENABLE_GAME_EDITOR_
	}

	void MainGame::Update(EngineTime time)
	{
	}

	void MainGame::FixedUpdate()
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
#endif // _ENABLE_GAME_EDITOR_
	}

	void MainGame::Uninitialize()
	{
	}
}