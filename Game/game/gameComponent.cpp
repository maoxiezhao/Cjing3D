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
#include "helper\fileSystem.h"
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
		auto& renderer = GlobalGetSubSystem<Renderer>();
		RenderPathForward* path = new RenderPathForward(renderer);
		renderer.SetCurrentRenderPath(path);

		Renderer2D& renderer2D = renderer.GetRenderer2D();
		renderer2D.SetCurrentRenderPath(path);

		// test load scene from archive
		//const std::string filePath = "Models/sandbox/sandbox.c3dscene";
		//renderer.GetMainScene().LoadSceneFromArchive(filePath);

		// test terrain
		//Scene& mainScene = renderer.GetMainScene();
		//auto terrainEntity = mainScene.CreateEntityTerrain("test", 512, 512, 50);
		//auto terrain = mainScene.mTerrains.GetComponent(terrainEntity);
		//terrain->LoadHeightMap("Textures/HeightMap.png");
		//terrain->LoadDetailMap(
		//	"Textures/TerrainWeights.dds", 
		//	"Textures/TerrainDetail1.dds",
		//	"Textures/TerrainDetail2.dds",
		//	"Textures/TerrainDetail3.dds"
		//);

		// test animations and gltf
		Scene& scene = Scene::GetScene();
		ModelImporter::ImportModelGLTF("Models/zombie/scene.gltf");
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
			ofn.lpstrFilter = "Scene file(Model file)\0*.c3dscene;*.obj\0";
			ofn.nFilterIndex = 1;
			ofn.lpstrFileTitle = nullptr;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = nullptr;

			// If you change folder in the dialog it will change the current folder for your process without OFN_NOCHANGEDIR;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;	

			auto a = FileData::IsFileExists("Textures/HeightMap.png");

			if (GetOpenFileNameA(&ofn))
			{
				std::string filePath = ofn.lpstrFile;
				std::string extension = FileData::GetExtensionFromFilePath(filePath);
				if (extension == ".c3dscene") {
					renderer.GetMainScene().LoadSceneFromArchive(filePath);
				}
				else if (extension == ".obj") {
					ModelImporter::ImportModelObj(filePath);
				}
				else if (extension == ".gltf") {
					ModelImporter::ImportModelGLTF(filePath);
				}
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
				MessageBoxA(NULL, TEXT("Scene saved successfully"), TEXT("Info"), MB_OK);
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