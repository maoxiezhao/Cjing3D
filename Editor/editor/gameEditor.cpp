#include "gameEditor.h"
#include "editor\guiEditor.h"

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

	void GameEditor::FixedUpdate()
	{
		GameComponent::FixedUpdate();

		auto inputManager = mEngine->GetInputManager();
		auto keyboard = inputManager->GetKeyBoard();
		if (keyboard->IsKeyPressed(KeyCode::W))
		{
			Logger::Info("Key press");
		}

		if (keyboard->IsKeyReleased(KeyCode::W))
		{
			Logger::Info("Key release");
		}

		if (keyboard->IsKeyHold(KeyCode::W))
		{
			Logger::Info("Key hold");
		}
	}
}