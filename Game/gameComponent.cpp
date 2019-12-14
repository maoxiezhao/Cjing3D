#include "gameComponent.h"
#include "resource\modelImporter.h"
#include "scripts\luaContext.h"
#include "renderer\renderer.h"
#include "renderer\renderer2D.h"
#include "system\sceneSystem.h"
#include "system\component\camera.h"
#include "input\InputSystem.h"
#include "helper\logger.h"
#include "renderer\paths\renderPath_forward.h"

namespace Cjing3D
{
	TestGame::TestGame()
	{
	}

	TestGame::~TestGame()
	{
	}

	void TestGame::Setup()
	{
	}

	void TestGame::Initialize()
	{
		auto systemContext = GetGameContext();
		ModelImporter::ImportModelObj("..\\Assets\\Models\\cornellbox.obj", *systemContext);

		auto& renderer = systemContext->GetSubSystem<Renderer>();
		RenderPathForward* path = new RenderPathForward(renderer);
		renderer.SetCurrentRenderPath(path);

		Renderer2D& renderer2D = renderer.GetRenderer2D();
		renderer2D.SetCurrentRenderPath(path);

		auto& mainScene = renderer.GetMainScene();
		mainScene.CreateEntityLight(
			"TestLight",
			{0.0f, 10.0f, -18.0f}
		);
	}

	void TestGame::Update(EngineTime time)
	{
		float moveSpeed = 10.0f;

		auto systemContext = GetGameContext();
		auto& renderer = systemContext->GetSubSystem<Renderer>();
		auto camera = renderer.GetCamera();

	}

	void TestGame::Uninitialize()
	{
	}
}