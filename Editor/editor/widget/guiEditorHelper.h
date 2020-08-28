#pragma once

#include "guiEditorWidget.h"
#include "system\sceneSystem.h"

namespace Cjing3D
{
	namespace EditorHelper
	{
		void LoadSceneFromOpenFile();
		void SaveSceneToOpenFile();
		void LoadSkyFromOpenFile();
		std::string GetFileNameFromOpenFile(const char* filter);
		std::string GetSceneEntityComboList(Scene& scene, const std::vector<ECS::Entity>& entityList, ECS::Entity currentEntity, int& currentIndex, bool haveNull = true);
	}
}