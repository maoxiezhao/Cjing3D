#pragma once

#include "editor\guiEditorInclude.h"

namespace Cjing3D
{
	class IMGUIStage;
	class Scene;

	namespace Editor
	{
		void LoadSceneFromOpenFile();
		void SaveSceneToOpenFile();
		void LoadSkyFromOpenFile();
		std::string GetFileNameFromOpenFile(const char* filter);
		std::string GetSceneEntityComboList(Scene& scene, const std::vector<ECS::Entity>& entityList, ECS::Entity currentEntity, int& currentIndex, bool haveNull = true);
	}
}