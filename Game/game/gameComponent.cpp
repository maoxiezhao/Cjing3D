#include "gameComponent.h"

#include "engine.h"
#include "resource\modelImporter.h"
#include "scripts\luaContext.h"
#include "renderer\renderer.h"
#include "renderer\renderer2D.h"
#include "system\sceneSystem.h"
#include "system\component\camera.h"
#include "input\InputSystem.h"
#include "helper\logger.h"
#include "renderer\paths\renderPath_forward.h"
#include "gui\guiStage.h"

#include <thread>
#include <Windows.h>

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
		//ModelImporter::ImportModelObj("Models/mechine/mechine.obj", *systemContext);

		auto& renderer = systemContext->GetSubSystem<Renderer>();
		RenderPathForward* path = new RenderPathForward(renderer);
		renderer.SetCurrentRenderPath(path);

		Renderer2D& renderer2D = renderer.GetRenderer2D();
		renderer2D.SetCurrentRenderPath(path);

		// test load scene from archive
		const std::string filePath = "Models/mechine/machine.c3dscene";
		renderer.GetMainScene().LoadSceneFromArchive(filePath);
	}

	void TestGame::Update(EngineTime time)
	{
		auto systemContext = GetGameContext();
		auto& inputManager = systemContext->GetSubSystem<InputManager>();
		auto& renderer = systemContext->GetSubSystem<Renderer>();

		if (inputManager.IsKeyDown(KeyCode::Esc)) {
			systemContext->GetEngine()->SetIsExiting(true);
		}

		if (inputManager.IsKeyDown(KeyCode::F4))
		{
			auto& guiStage = systemContext->GetSubSystem<GUIStage>();
			bool show = guiStage.IsShowImGUIDetailInfo();
			guiStage.ShowImGUIDetailInfo(!show);
		}

		// test scene serializing
		if (inputManager.IsKeyDown(KeyCode::F5)) 
		{
			char szFile[260] = { '\0' };

			OPENFILENAMEA ofn;
			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = nullptr;
			ofn.lpstrFile = szFile;
			ofn.nMaxFile = sizeof(szFile);
			ofn.lpstrFilter = "Scene file\0*.c3dscene\0";
			ofn.nFilterIndex = 1;
			ofn.lpstrFileTitle = nullptr;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = nullptr;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

			if (GetOpenFileNameA(&ofn))
			{
				std::string filePath = ofn.lpstrFile;
				renderer.GetMainScene().LoadSceneFromArchive(filePath);
			}
		}
		else if (inputManager.IsKeyDown(KeyCode::F6)) 
		{
			char szFile[260] = {'\0'};

			OPENFILENAMEA ofn;
			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = nullptr;
			ofn.lpstrFile = szFile;
			ofn.nMaxFile = sizeof(szFile);
			ofn.lpstrFilter = "Scene file\0*.c3dscene\0";
			ofn.nFilterIndex = 1;
			ofn.lpstrFileTitle = nullptr;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = nullptr;
			ofn.Flags = OFN_OVERWRITEPROMPT;

			if (GetSaveFileNameA(&ofn))
			{
				std::string filePath = ofn.lpstrFile;
				if (filePath.find(".c3dscene") == std::string::npos) {
					filePath = filePath + ".c3dscene";
				}
				renderer.GetMainScene().SaveSceneToArchive(filePath);
			}
		}
		else if (inputManager.IsKeyDown(KeyCode::F7))
		{
			renderer.GetMainScene().Clear();
		}
	}

	void TestGame::Uninitialize()
	{
	}
}