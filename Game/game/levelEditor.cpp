#include "levelEditor.h"
#include "engine.h"
#include "renderer\paths\renderPath_tiledForward.h"
#include "editor\guiEditor.h"
#include "system\sceneSystem.h"

using namespace Cjing3D;

namespace CjingGame
{
	namespace {
		U32 mCurrentLayer = 0;
		F32x3 mGridOffset = F32x3(0.0f, 0.0f, 0.0f);
		Scene::PickResult mPickResult;
	}

	LevelEditor::LevelEditor()
	{
	}

	LevelEditor::~LevelEditor()
	{
	}

	void LevelEditor::Initialize()
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

		// init game luaContext
		mGameLuaContext = std::make_unique<GameLuaContext>();
		mGameLuaContext->Initialize();

		GlobalGetSubSystem<LuaContext>().ChangeLuaScene("LevelEditor");

		F32 cellSize = (F32)GameMap::MapCellSize;
		InitializeEditorMap(cellSize, 8, 8, 4);
		InitializeEditorGUI(guiStage.GetImGUIStage());
	}

	void LevelEditor::Update(Cjing3D::EngineTime time)
	{
	}

	void LevelEditor::FixedUpdate()
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
		mGameLuaContext->FixedUpdate();

		// update editor
		mGridOffset[1] = (F32)mCurrentLayer;
		Renderer::SetDebugGridOffset(mGridOffset);

		if (mCurrentMap != nullptr) 
		{
			// update editor
			switch (mEditorMode)
			{
			case CjingGame::EditorMode_Ground:
				UpdateEditorGround();
				break;
			case CjingGame::EditorMode_Wall:
				UpdateEditorWall();
				break;
			default:
				break;
			}

			mCurrentMap->FixedUpdate();
		}
	}

	void LevelEditor::Uninitialize()
	{
		if (mCurrentMap != nullptr) {
			mCurrentMap->Uninitialize();
			mCurrentMap = nullptr;
		}

		mGameLuaContext->Uninitialize();
		mEditorPlane.RemoveFromScene();

		Scene::GetScene().Clear();
	}

	void LevelEditor::InitializeEditorMap(F32 cellSize, I32 width, I32 height, I32 layer)
	{
		Renderer::SetDebugGridSize({ width, height });
		mGridOffset = {
			width / 2 * cellSize, 
			0.0f,
			height /2 * cellSize
		};

		mEditorPlane.LoadFromScene(Scene::GetScene());
		mEditorPlane.LoadModel("Models/planeNormalize.obj");
		mEditorPlane.SetScale({ (F32)width, 1.0f, (F32)height });
		mEditorPlane.SetPosition(mGridOffset);
		mEditorPlane.SetVisible(false);

		mCurrentMap = std::make_unique<GameMap>();
		mCurrentMap->Initialize(width, height, layer);
	}

	void LevelEditor::UpdateEditorGround()
	{
		auto& inputManager = GlobalGetSubSystem<InputManager>();
		Scene& scene = Scene::GetScene();
		I32x2 mousePos = inputManager.GetMousePos();
		mPickResult = scene.MousePickObjects({ (U32)mousePos[0], (U32)mousePos[1] }, GameObjectLayerMask_Ground);

		if (inputManager.IsKeyDown(KeyCode::Click_Left))
		{
			Logger::Info(std::to_string(mPickResult.entity));
			if (mPickResult.entity == INVALID_ENTITY) {
				return;
			}

		/*	I32x3 localPos = */

		}
		else if (inputManager.IsKeyDown(KeyCode::Click_Right))
		{
			Logger::Info(std::to_string(mPickResult.entity));
		}
	}

	void LevelEditor::UpdateEditorWall()
	{
		auto& inputManager = GlobalGetSubSystem<InputManager>();
		Scene& scene = Scene::GetScene();
		I32x2 mousePos = inputManager.GetMousePos();
		mPickResult = scene.MousePickObjects({ (U32)mousePos[0], (U32)mousePos[1] }, GameObjectLayerMask_Wall);

		if (inputManager.IsKeyDown(KeyCode::Click_Left))
		{
			Logger::Info(std::to_string(mPickResult.entity));
		}
	}
}