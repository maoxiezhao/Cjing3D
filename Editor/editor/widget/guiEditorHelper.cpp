#include "guiEditorHelper.h"

#include "helper\fileSystem.h"
#include "system\sceneSystem.h"
#include "resource\resourceManager.h"
#include "platform\platform.h"

namespace Cjing3D
{
namespace EditorHelper
{
	void LoadSceneFromOpenFile()
	{
		Platform::LoadFileFromOpenWindow(
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
		Platform::SaveFileToOpenWindow(
			"Scene file\0*.c3dscene\0",
			[](const std::string& filePath) {

				std::string path = filePath;
				if (path.find(".c3dscene") == std::string::npos) {
					path = path + ".c3dscene";
				}
				Scene::GetScene().SaveSceneToArchive(path);
				Platform::ShowMessageBox("Scene saved successfully");
			});
	}

	void LoadSkyFromOpenFile()
	{
		Platform::LoadFileFromOpenWindow(
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
		Platform::LoadFileFromOpenWindow(
			filter,
			[&ret](const std::string& filePath) {
				ret = filePath;
			});

		return ret;
	}

	std::string GetSceneEntityComboList(Scene& scene, const std::vector<ECS::Entity>& entityList, ECS::Entity currentEntity, int& currentIndex, bool haveNull)
	{
		std::string entityNameList = "\0";
		if (haveNull)
		{
			currentIndex = 0;
			entityNameList = entityNameList + "NULL" + '\0';
		}

		for (int index = 0; index < entityList.size(); index++)
		{
			Entity entity = entityList[index];
			if (entity == currentEntity) {
				currentIndex = haveNull ? index + 1 : index;
			}

			std::string nodeName = "Entity ";
			auto nameComponent = scene.mNames.GetComponent(entity);
			if (nameComponent != nullptr) {
				nodeName = nodeName + " " + nameComponent->GetString();
			}
			else {
				nodeName = nodeName + std::to_string(entity);
			}

			entityNameList = entityNameList + nodeName + '\0';
		}

		return entityNameList;
	}
}
}