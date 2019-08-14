#include "gameComponent.h"
#include "resource\modelImporter.h"
#include "scripts\luaContext.h"
#include "renderer\renderer.h"
#include "system\component\camera.h"
#include "core\InputSystem.h"
#include "helper\logger.h"

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
		ModelImporter::ImportModelObj("..\\Assets\\Models\\cornell_box.obj", *systemContext);

		auto& renderer = systemContext->GetSubSystem<Renderer>();
		auto camera = renderer.GetCamera();
		camera->SetCameraStatus(
			{ 250.0f, 200.0f, -250.0f },
			{ 0.0f, 0.0f, 1.0f },
			{ 0.0f, 1.0f, 0.0f }
		);

		auto& luaContext = systemContext->GetSubSystem<LuaContext>();
		luaContext.DoFileIfExists("Scripts/main");
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