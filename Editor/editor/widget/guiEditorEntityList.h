#pragma once

#include "guiEditorWidget.h"
#include "guiEditorHelper.h"
#include "system\sceneSystem.h"

namespace Cjing3D
{
	class EditorWidgetEntityList : public EditorWidget
	{
	public:
		EditorWidgetEntityList(EditorStage& imguiStage);

		void Update(F32 deltaTime)override;
		void SetSelectedEntity(ECS::Entity entity);
		void DeleteEntity(ECS::Entity entity);

	private:
		void TreeAddEntity(ECS::Entity entity);
		void UpdateInput();
		void UpdateContextMenu();
		void UpdateRenamePopup();
		
		ECS::Entity EntityCreateEmpty(const std::string& name);
		ECS::Entity EntityCreateDirectionalLight();
		ECS::Entity EntityCreatePointLight();
		ECS::Entity EntityCreateParticle();
		ECS::Entity EntityCreateSound();
		ECS::Entity EntityCreateTerrain();

	private:
		Scene& mScene;
		ECS::Entity mHoveredEntity = INVALID_ENTITY;
		ECS::Entity mClickedEntity = INVALID_ENTITY;
		ECS::Entity mCopiedEntity  = INVALID_ENTITY;
		bool mShowRenamePopup = false;
		EditorDragPayload mDragPayload;

		std::vector<ECS::Entity> mRootEntities;
		std::map<ECS::Entity, std::vector<ECS::Entity>> mEntityAttachedChildrenMap;
	};
}