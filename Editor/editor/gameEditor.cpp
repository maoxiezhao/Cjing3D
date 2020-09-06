#include "gameEditor.h"
#include "editor\guiEditor.h"
#include "core\eventSystem.h"
#include "renderer\renderer.h"
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

		mEditorStage = std::make_unique<EditorStage>(*mGameWindow.get());
		mEditorStage->Initialize();
	}

	void GameEditor::Uninitialize()
	{
		mEditorStage->Uninitialize();

		GameComponent::Uninitialize();
	}

	void GameEditor::FixedUpdate()
	{
	}

	void GameEditor::Compose()
	{
		PROFILER_BEGIN_CPU_BLOCK("Compose");
		auto& device = Renderer::GetDevice();
		CommandList cmd = device.GetCommandList();

		if (mCurrentRenderPath != nullptr) 
		{
			auto viewportRenderTarget = mEditorStage->GetViewportRenderTarget();
			device.BindRenderTarget(cmd, 1, &viewportRenderTarget, nullptr);
			device.ClearRenderTarget(cmd, *viewportRenderTarget, { 0.0f, 0.0f, 0.0f, 1.0f });

			mCurrentRenderPath->Compose(cmd);
		}

		device.PresentBegin(cmd);
		mEditorStage->Render(cmd);
		device.PresentEnd(cmd);
		PROFILER_END_BLOCK();
	}

	void GameEditor::EndFrame()
	{
		mEditorStage->EndFrame();
		GameComponent::EndFrame();
	}
}