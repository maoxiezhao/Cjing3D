#include "levelEditor.h"
#include "engine.h"
#include "renderer\paths\renderPath_tiledForward.h"
#include "editor\guiEditor.h"
#include "system\sceneSystem.h"
#include "gui\guiEditor\guiEditorInclude.h"
#include "scripts\gameLuaContext.h"
#include "game\map\gameMapRenderer.h"

using namespace Cjing3D;

namespace CjingGame
{
	namespace {
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

		// map renderer
		GameMapRenderer::Initialize();

		// initialize game luaContext
		mGameLuaContext = std::make_unique<GameLuaContext>();
		mGameLuaContext->Initialize();

		GlobalGetSubSystem<LuaContext>().ChangeLuaScene("LevelEditor");

		F32 cellSize = (F32)GameContext::MapCellSize;
		InitializeEditorMap("", cellSize, 8, 8, 4);
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

		// update editor mode
		if (mCurrentMap != nullptr) 
		{
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

		// update editor layer
		if (inputManager.IsKeyDown(KeyCode::Q))
		{
			mCurrentLayer = std::max(0, mCurrentLayer - 1);
		}
		else if (inputManager.IsKeyDown(KeyCode::E))
		{
			I32 maxLayer = mCurrentMap != nullptr ? mCurrentMap->GetMapLayer() - 1 : 0;
			mCurrentLayer = std::min(maxLayer, mCurrentLayer + 1);
		}
		if ((I32)mGridOffset[1] != mCurrentLayer) 
		{
			mGridOffset[1] = (F32)mCurrentLayer;
			Renderer::SetDebugGridOffset(mGridOffset);
			mEditorPlane.SetPosition(mGridOffset);
		}
	}

	void LevelEditor::Uninitialize()
	{
		if (mCurrentMap != nullptr) {
			mCurrentMap->Clear();
			mCurrentMap = nullptr;
		}

		mGameLuaContext->Uninitialize();
		mEditorPlane.RemoveFromScene();

		// map renderer
		GameMapRenderer::Uninitialize();

		Scene::GetScene().Clear();
	}

	void LevelEditor::PreRender()
	{
		if (mCurrentMap != nullptr) {
			mCurrentMap->PreRender();
		}

		// map renderer
		GameMapRenderer::PreRender();
	}

	void LevelEditor::SetDebugGridVisible(bool visible)
	{
		if (visible == mIsDebugGridVisible) {
			return;
		}
		mIsDebugGridVisible = visible;
		
		if (visible == false || mCurrentMap == nullptr)
		{
			Renderer::SetDebugGridSize({ 0, 0 });
		}
		else
		{
			I32 width = mCurrentMap->GetMapWidth();
			I32 height = mCurrentMap->GetMapHeight();	
			Renderer::SetDebugGridSize({ width, height });
			Renderer::SetDebugGridOffset(mGridOffset);
		}
	}

	I32x3 LevelEditor::GetCurrentPickLocalPos() const
	{
		if (mPickResult.entity == INVALID_ENTITY || mCurrentMap == nullptr) {
			return I32x3(-1, -1, -1);
		}
	
		return mCurrentMap->TransformGlobalPosToLocal(mPickResult.position);
	}

	void LevelEditor::NewMap(const std::string& mapName, I32 h, I32 w, I32 layer)
	{
		if (mCurrentMap != nullptr) {
			mCurrentMap->Clear();
			mCurrentMap.reset();
		}

		InitializeEditorMap(mapName, GameContext::MapCellSize, w, h, layer);
	}

	void LevelEditor::SaveMap(const std::string& mapPath)
	{
		if (mCurrentMap != nullptr) {
			mCurrentMap->SaveByParentPath(mapPath);
		}
	}

	void LevelEditor::LoadMap(const std::string& mapPath)
	{
		if (mCurrentMap != nullptr) {
			mCurrentMap->Clear();
			mCurrentMap.reset();
		}

		mCurrentMap = std::make_unique<GameMap>(mapPath);
		mCurrentMap->LoadMapParts({ 0, 0, 0 });
	}

	void LevelEditor::InitializeEditorMap(const std::string& mapName, F32 cellSize, I32 width, I32 height, I32 layer)
	{
		mGridOffset = {
			width / 2 * cellSize,
			0.0f,
			height / 2 * cellSize
		};

		mEditorPlane.LoadFromScene(Scene::GetScene());
		mEditorPlane.SetMeshFromModel("Models/planeNormalize.obj");
		mEditorPlane.SetScale({ (F32)width, 1.0f, (F32)height });
		mEditorPlane.SetPosition(mGridOffset);
		mEditorPlane.SetVisible(false);

		mCurrentMap = std::make_unique<GameMap>(width, height, layer, mapName);
		mCurrentMap->LoadMapParts({0, 0, 0});

		SetDebugGridVisible(true);
	}

	void LevelEditor::UpdateEditorGround()
	{
		UpdateRaycast(GameObjectLayerMask_Ground);
		if (mPickResult.entity == INVALID_ENTITY) {
			return;
		}

		auto& inputManager = GlobalGetSubSystem<InputManager>();
		if (inputManager.IsKeyDown(KeyCode::Click_Left))
		{
			if (ImGui::IsAnyWindowFocused()) {
				return;
			}

			I32x3 localPos = mCurrentMap->TransformGlobalPosToLocal(mPickResult.position);
			mCurrentMap->AddGround(localPos, mCurrentGroundTilesetIndex);
		}
		else if (inputManager.IsKeyDown(KeyCode::Click_Right))
		{
			if (ImGui::IsAnyWindowFocused()) {
				return;
			}

			I32x3 localPos = mCurrentMap->TransformGlobalPosToLocal(mPickResult.position);
			mCurrentMap->RemoveGround(localPos);
		}
	}

	void LevelEditor::UpdateEditorWall()
	{
		UpdateRaycast(GameObjectLayerMask_Wall);
		if (mPickResult.entity == INVALID_ENTITY) {
			return;
		}

		auto& inputManager = GlobalGetSubSystem<InputManager>();
		if (inputManager.IsKeyDown(KeyCode::Click_Left))
		{
			Logger::Info(std::to_string(mPickResult.entity));
		}
	}

	void LevelEditor::UpdateRaycast(const GameObjectLayerMask mask)
	{
		Scene& scene = Scene::GetScene();
		auto& inputManager = GlobalGetSubSystem<InputManager>();
		I32x2 mousePos = inputManager.GetMousePos();
		Ray cameraRay = Renderer::GetMainCameraMouseRay({ (U32)mousePos[0], (U32)mousePos[1] });

		// 先检测current editor plane
		std::vector<ECS::Entity> pickedObjects;
		pickedObjects.push_back(mEditorPlane.GetEntity());
		Scene::PickResult planeResult = scene.PickObjects(pickedObjects, cameraRay);

		// 获取相机射线所穿过的所有mapParts
		auto mapParts = mCurrentMap->GetMapPartsByRay(cameraRay);
		for (auto mapPart : mapParts)
		{

		}

		mPickResult = planeResult;
	}
}