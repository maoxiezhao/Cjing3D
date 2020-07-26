#pragma once

#include "core\gameComponent.h"
#include "scripts\gameLuaContext.h"
#include "game\map\gameMap.h"
#include "game\gameObject.h"
#include "gui\guiStage.h"

namespace CjingGame
{
	enum EditorMode
	{
		EditorMode_Ground,
		EditorMode_Wall,
	};

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

		EditorMode GetEditorMode() const { return mEditorMode; };
		void SetEditorMode(EditorMode mode) { mEditorMode = mode; }
		void SetDebugGridVisible(bool visible);
		bool IsDebugGridVisible()const { return mIsDebugGridVisible; }

	private:
		void InitializeEditorMap(F32 cellSize, I32 width, I32 height, I32 layer);
		void InitializeEditorGUI(Cjing3D::IMGUIStage& imguiStage);

		// mode update functions
		void UpdateEditorGround();
		void UpdateEditorWall();

	private:
		std::unique_ptr<GameLuaContext> mGameLuaContext = nullptr;
		std::unique_ptr<GameMap> mCurrentMap = nullptr;
		EditorMode mEditorMode = EditorMode_Ground;
		GameObject mEditorPlane;
		bool mIsDebugGridVisible = false;
	};

}