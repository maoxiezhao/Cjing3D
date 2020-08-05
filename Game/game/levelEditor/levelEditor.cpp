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
		I32x3 mPrevMousePos = I32x3(-1, -1, -1);
		GameMapGround* mPickedMapGround = nullptr;

		F32x3 mCameraOffset = F32x3(0.0f, 20.0f, -5.0f);
		TransformComponent mCameraTransform;
	}

	LevelEditor::LevelEditor() :
		mEditorPlane("EditorPlane"),
		mEditorCursor(I32x3(0, 0, 0))
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

		// do lua script
		GlobalGetSubSystem<LuaContext>().ChangeLuaScene("LevelEditor");

		// init cursor and editor plane
		mEditorCursor.LoadFromScene(Scene::GetScene());
		mEditorCursor.LoadCursor("Models/Editor/cursor.obj");

		mEditorPlane.LoadFromScene(Scene::GetScene());
		mEditorPlane.SetMeshFromModel("Models/planeNormalize.obj");
		
		// init camera
		mCameraTransform.SetRotateFromRollPitchYaw({ 1.2f, 0.0f, 0.0f });
		UpdateCamera();

		// init default map
		F32 cellSize = (F32)GameContext::MapCellSize;
		InitializeEditorMap("", cellSize, 48, 8, 4);
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

		if (mCurrentMap == nullptr) {
			return;
		}

		// update cursor 
		UpdateCursor();

		// update editor mode
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
		mEditorCursor.RemoveFromScene();

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
		mCurrentMap->LoadMapParts(mEditorCursor.GetMapPartPos());
	}

	void LevelEditor::UpdateCursor()
	{
		if (mCurrentMap == nullptr) {
			return;
		}

		I32x3 localPos = mEditorCursor.GetLocalPos();
		I32 mapWidth = mCurrentMap->GetMapWidth();
		I32 mapHeight = mCurrentMap->GetMapHeight();

		auto& inputManager = GlobalGetSubSystem<InputManager>();
		if (inputManager.IsKeyDown(KeyCode::D))
		{
			localPos[0] = std::min(mapWidth - 1, localPos[0] + 1);
		}
		else if (inputManager.IsKeyDown(KeyCode::A))
		{
			localPos[0] = std::max(0, localPos[0] - 1);
		}
		else if (inputManager.IsKeyDown(KeyCode::W))
		{
			localPos[2] = std::min(mapHeight - 1, localPos[2] + 1);
		}
		else if (inputManager.IsKeyDown(KeyCode::S))
		{
			localPos[2] = std::max(0, localPos[2] - 1);
		}

		if (mEditorCursor.GetLocalPos() != localPos)
		{
			mEditorCursor.SetLocalPos(localPos);
			mEditorCursor.UpdatePosition(mCurrentMap.get());
			mCurrentMap->SetCurrentPartPos(mEditorCursor.GetMapPartPos());

			// update editor camera
			UpdateCamera();
		}
	}

	void LevelEditor::UpdateCamera()
	{
		if (mCameraMode != CameraMode_TraceCursor) {
			return;
		}

		F32x3 cursorPos = F32x3(0.0f, 0.0f, 0.0f);
		if (mCurrentMap != nullptr) {
			cursorPos = mCurrentMap->TransformLocalPosToGlobal(mEditorCursor.GetLocalPos());
		}			
		cursorPos += mCameraOffset;
	
		mCameraTransform.SetTranslationLocal(XMConvert(cursorPos));	
		mCameraTransform.Update();
		Renderer::GetCamera().Transform(mCameraTransform);
	}

	void LevelEditor::InitializeEditorMap(const std::string& mapName, F32 cellSize, I32 width, I32 height, I32 layer)
	{
		mGridOffset = {
			width / 2 * cellSize,
			0.0f,
			height / 2 * cellSize
		};

		mEditorPlane.SetScale({ (F32)width, 1.0f, (F32)height });
		mEditorPlane.SetPosition(mGridOffset);
		mEditorPlane.SetVisible(false);

		mCurrentMap = std::make_unique<GameMap>(width, height, layer, mapName);
		mCurrentMap->LoadMapParts(mEditorCursor.GetMapPartPos());

		SetDebugGridVisible(true);
	}

	void LevelEditor::UpdateEditorGround()
	{
		UpdateRaycast(GameObjectLayerMask_Ground);
		if (mPickResult.entity == INVALID_ENTITY) {
			return;
		}

		auto& inputManager = GlobalGetSubSystem<InputManager>();
		if (!inputManager.IsKeyDown(KeyCode::Click_Left) &&
			!inputManager.IsKeyDown(KeyCode::Click_Right)) {
			return;
		}

		if (ImGui::IsAnyWindowHovered()) {
			return;
		}

		I32x3 localPos = mCurrentMap->TransformGlobalPosToLocal(mPickResult.position);
		if (localPos != mPrevMousePos)
		{
			mPrevMousePos = localPos;

			if (inputManager.IsKeyDown(KeyCode::Click_Left)) {
				mCurrentMap->AddGround(localPos, mCurrentGroundTilesetIndex);
			}
			else {
				mCurrentMap->RemoveGround(localPos);
			}
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
		Scene::PickResult pickResult;
		auto mapPartsPos = mCurrentMap->GetMapPartsPosByRay(cameraRay);
		for (auto mapPartPos : mapPartsPos)
		{
			auto mapPart = mCurrentMap->GetMapPartByLocalPartPos(mapPartPos);
			if (mapPart != nullptr)
			{
				switch (mask)
				{
				case GameObjectLayerMask_Ground:
					mPickedMapGround = mapPart->RaycastMapGround(cameraRay, pickResult);
					break;
				case GameObjectLayerMask_Wall:
					break;
				default:
					break;
				}
			}
		}

		if (pickResult.entity == INVALID_ENTITY || 
			pickResult.position.y() < planeResult.position.y())
		{
			pickResult = planeResult;
			mPickedMapGround = nullptr;
		}

		mPickResult = pickResult;
	}

}