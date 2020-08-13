#include "guiEditorScene.h"

#include "helper\fileSystem.h"
#include "system\sceneSystem.h"
#include "resource\resourceManager.h"
#include "platform\gameWindow.h"

#include <thread>
#include <Windows.h>

namespace Cjing3D
{
namespace Editor
{
	void LoadSceneFromOpenFile()
	{
		GameWindow::LoadFileFromOpenWindow(
			"Scene file(Model file)\0*.c3dscene;*.obj;*.gltf\0",
			[](const std::string& filePath) {

				std::string extension = FileData::GetExtensionFromFilePath(filePath);
				if (extension == ".c3dscene") {
					Scene::GetScene().LoadSceneFromArchive(filePath);
				}
				else if (extension == ".obj") {
					Scene scene;
					ModelImporter::ImportModelObj(filePath, scene);
					Scene::GetScene().Merge(scene);
				}
				else if (extension == ".gltf") {
					Scene scene;
					ModelImporter::ImportModelGLTF(filePath, scene);
					Scene::GetScene().Merge(scene);
				}
			});

	}

	void SaveSceneToOpenFile()
	{
		GameWindow::SaveFileToOpenWindow(
			"Scene file\0*.c3dscene\0",
			[](const std::string& filePath) {

				std::string path = filePath;
				if (path.find(".c3dscene") == std::string::npos) {
					path = path + ".c3dscene";
				}
				Scene::GetScene().SaveSceneToArchive(path);
				GameWindow::ShowMessageBox("Scene saved successfully");
			});
	}

	void LoadSkyFromOpenFile()
	{
		GameWindow::LoadFileFromOpenWindow(
			"Cube map\0*.dds\0",
			[](const std::string& filePath) {

				std::string extension = FileData::GetExtensionFromFilePath(filePath);
				if (extension == ".dds") {
					ECS::Entity entity = INVALID_ENTITY;
					Scene& scene = Scene::GetScene();
					if (scene.mWeathers.Empty()) {
						entity = scene.CreateWeather("DefaultWeather");
					}
					else {
						entity = scene.mWeathers.GetEntityByIndex(0);
					}

					auto weather = scene.mWeathers.GetComponent(entity);
					if (weather != nullptr)
					{
						weather->LoadSkyMap(filePath);
					}
				}
			});
	}

	std::string GetFileNameFromOpenFile(const char* filter)
	{
		std::string ret;
		GameWindow::LoadFileFromOpenWindow(
			filter,
			[&ret](const std::string& filePath) {
				ret = filePath;
			});

		return ret;
	}
}
}