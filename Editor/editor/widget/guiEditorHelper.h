#pragma once

#include "guiEditorWidget.h"
#include "system\sceneSystem.h"

namespace Cjing3D
{
	enum EditorDragType
	{
		EditorDragType_Unknown,
		EditorDragType_Entity
	};

	struct EditorDragPayload
	{
		ECS::Entity entity = INVALID_ENTITY;
		EditorDragType type = EditorDragType_Unknown;
	};

	namespace EditorHelper
	{
		// scene functions
		void LoadSceneFromOpenFile();
		void SaveSceneToOpenFile();
		void LoadSkyFromOpenFile();
		std::string GetFileNameFromOpenFile(const char* filter);
		std::string GetSceneEntityComboList(Scene& scene, const std::vector<ECS::Entity>& entityList, ECS::Entity currentEntity, int& currentIndex, bool haveNull = true);
	
		// entity function
		ECS::Entity GetSelectedEntity();
		void SetSelectedEntity(ECS::Entity entity);
		void SetDragDropPayload(EditorDragPayload& payLoad);
		EditorDragPayload* AcceptDragDropPayload(EditorDragType type);
	}
}