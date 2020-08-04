#pragma once

#include "game\levelEditor\levelEditorUtils.h"
#include "game\map\gameMap.h"
#include "scripts\gameLuaContext.h"

namespace CjingGame
{
	class LevelEditor : public Cjing3D::GameComponent
	{
	public:
		LevelEditor();
		virtual ~LevelEditor();

		virtual void Initialize();
		virtual void Update(Cjing3D::EngineTime time);
		virtual void FixedUpdate();
		virtual void Uninitialize();
		virtual void PreRender();

		//////////////////////////////////////////////////////////////////////////////
		// general editor
		EditorMode GetEditorMode() const { return mEditorMode; };
		void SetEditorMode(EditorMode mode) { mEditorMode = mode; }
		void SetDebugGridVisible(bool visible);
		bool IsDebugGridVisible()const { return mIsDebugGridVisible; }
		I32 GetCurrentLayer()const { return mCurrentLayer; }
		void SetCurrentLayer(I32 layer) { mCurrentLayer = layer; }
		I32x3 GetCurrentPickLocalPos()const;
		GameMap* GetCurrentMap() { return mCurrentMap != nullptr ? mCurrentMap.get() : nullptr; }

		void NewMap(const std::string& mapName, I32 w, I32 h, I32 layer);
		void SaveMap(const std::string& mapPath);
		void LoadMap(const std::string& mapPath);

		EditorMode mEditorMode = EditorMode_Ground;
		GameObject mEditorPlane;
		bool mIsDebugGridVisible = false;
		I32 mCurrentLayer = 0;
		EditorCursor mEditorCursor;

		void UpdateCursor();
		void UpdateCamera();
		CameraMode mCameraMode = CameraMode_TraceCursor;

		//////////////////////////////////////////////////////////////////////////////
		// ground editor
		U32 GetGroundTilesetIndex()const { return mCurrentGroundTilesetIndex; }
		void SetCurrentGroundTilestIndex(U32 index) { mCurrentGroundTilesetIndex = index; }

		U32 mCurrentGroundTilesetIndex = 0;

	private:
		void InitializeEditorMap(const std::string& mapName, F32 cellSize, I32 width, I32 height, I32 layer);
		void InitializeEditorGUI(Cjing3D::IMGUIStage& imguiStage);

		// mode update functions
		void UpdateEditorGround();
		void UpdateEditorWall();
		void UpdateRaycast(const GameObjectLayerMask mask);

	private:
		std::unique_ptr<GameLuaContext> mGameLuaContext = nullptr;
		std::unique_ptr<GameMap> mCurrentMap = nullptr;
	};

}