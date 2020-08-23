#include "gameEditor.h"
#include "engine.h"
#include "scripts\luaContext.h"
#include "system\sceneSystem.h"
#include "renderer\paths\renderPath_tiledForward.h"
#include "gui\guiStage.h"
#include "gui\imguiStage.h"
#include "renderer\renderer.h"
#include "editor\guiEditor.h"

namespace Cjing3D
{
	std::unique_ptr<EditorStage> mEditorStage = nullptr;

	GameEditor::GameEditor()
	{
	}

	GameEditor::~GameEditor()
	{
	}

	void GameEditor::Initialize()
	{
		// init renderer
		RenderPathTiledForward* path = new RenderPathTiledForward();
		Renderer::SetCurrentRenderPath(path);
		Font::LoadFontTTF("Fonts/arial.ttf");

		// init editor
		mEditorStage = std::make_unique<EditorStage>(GlobalGetSubSystem<GUIStage>().GetGUIRenderer());
		mEditorStage->Initialize();

		// change lua scene
		GlobalGetSubSystem<LuaContext>().ChangeLuaScene("EditorScene");
	}

	void GameEditor::Update(EngineTime time)
	{
	}

	void GameEditor::FixedUpdate()
	{
		auto& inputManager = GlobalGetSubSystem<InputManager>();
		if (inputManager.IsKeyDown(KeyCode::Esc)) {
			GlobalGetEngine()->SetIsExiting(true);
		}

		mEditorStage->FixedUpdate();
	}

	void GameEditor::Uninitialize()
	{
		mEditorStage->Uninitialize();
		mEditorStage = nullptr;
	}

	void GameEditor::EndFrame()
	{
		mEditorStage->EndFrame();
	}
}