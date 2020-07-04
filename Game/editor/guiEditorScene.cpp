#include "guiEditorScene.h"

#include "helper\fileSystem.h"
#include "system\sceneSystem.h"
#include "resource\resourceManager.h"

#include <thread>
#include <Windows.h>

namespace Cjing3D
{
namespace Editor
{
	void LoadSceneFromOpenFile()
	{
		char szFile[260] = { '\0' };

		OPENFILENAMEA ofn;
		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = nullptr;
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = "Scene file(Model file)\0*.c3dscene;*.obj;*.gltf\0";
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
				Scene::GetScene().LoadSceneFromArchive(filePath);
			}
			else if (extension == ".obj") {
				ModelImporter::ImportModelObj(filePath);
			}
			else if (extension == ".gltf") {
				ModelImporter::ImportModelGLTF(filePath);
			}
		}
	}

	void SaveSceneToOpenFile()
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
		ofn.Flags = OFN_OVERWRITEPROMPT;

		if (GetSaveFileNameA(&ofn))
		{
			std::string filePath = ofn.lpstrFile;
			if (filePath.find(".c3dscene") == std::string::npos) {
				filePath = filePath + ".c3dscene";
			}
			Scene::GetScene().SaveSceneToArchive(filePath);
			MessageBoxW(NULL, TEXT("Scene saved successfully"), TEXT("Info"), MB_OK);
		}
	}
}
}