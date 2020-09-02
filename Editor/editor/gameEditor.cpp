#include "gameEditor.h"
#include "editor\guiEditor.h"
#include "core\eventSystem.h"
#include "renderer\paths\renderPath_tiledForward.h"

namespace Cjing3D
{
	std::unique_ptr<EditorStage> mEditorStage = nullptr;

	GameEditor::GameEditor(const std::shared_ptr<Engine>& engine) :
		GameComponent(engine)
	{
	}

	GameEditor::~GameEditor()
	{
	}

	void GameEditor::Initialize()
	{
		GameComponent::Initialize();

		auto renderPath = CJING_MAKE_SHARED<RenderPathTiledForward>();
		renderPath->Initialize();
		SetRenderPath(renderPath);
	}

	void GameEditor::FixedUpdate()
	{
		auto inputManager = mEngine->GetInputManager();
		if (inputManager->IsKeyPressed(KeyCode::W))
		{
			Logger::Info("Key press");
		}

		if (inputManager->IsKeyReleased(KeyCode::W))
		{
			Logger::Info("Key release");
		}

		if (inputManager->IsKeyHold(KeyCode::W))
		{
			Logger::Info("Key hold");
		}

		if (inputManager->IsKeyPressed(KeyCode::Click_Right))
		{
			Logger::Info("Key hold");
		}

		if (inputManager->IsKeyPressed(KeyCode::Gamepad_A))
		{
			Logger::Info("Key Pressed");
		}
	}
}