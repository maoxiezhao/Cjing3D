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

		mEditorStage = std::make_unique<EditorStage>(mGameWindow.get());
		mEditorStage->Initialize();
	}

	void GameEditor::Uninitialize()
	{
		if (mEditorStage != nullptr) {
			mEditorStage->Uninitialize();
		}

		GameComponent::Uninitialize();
	}

	void GameEditor::FixedUpdate()
	{
	}
}