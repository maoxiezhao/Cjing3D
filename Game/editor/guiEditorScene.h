#pragma once

#include "editor\guiEditorInclude.h"

namespace Cjing3D
{
	class IMGUIStage;

	namespace Editor
	{
		void LoadSceneFromOpenFile();
		void SaveSceneToOpenFile();
		void LoadSkyFromOpenFile();
		std::string GetFileNameFromOpenFile(const char* filter);
	}
}