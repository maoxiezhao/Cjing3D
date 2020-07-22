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

		//auto entity = Scene::GetScene().CreateWeather("Default");
		//auto weather = Scene::GetScene().mWeathers.GetComponent(entity);
		//weather->LoadSkyMap(
		//	{2048, 2048},
		//	{ 
		//	"Textures/skybox/CloudyCrown_Midnight_Right.png",
		//	"Textures/skybox/CloudyCrown_Midnight_Left.png",
		//	"Textures/skybox/CloudyCrown_Midnight_Up.png",
		//	"Textures/skybox/CloudyCrown_Midnight_Down.png",
		//	"Textures/skybox/CloudyCrown_Midnight_Front.png",
		//	"Textures/skybox/CloudyCrown_Midnight_Back.png" 
		//	}
		//);
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