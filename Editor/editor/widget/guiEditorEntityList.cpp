#include "guiEditorEntityList.h"
#include "guiEditorInclude.h"
#include "guiEditorProperties.h"

namespace Cjing3D
{
	EditorWidgetEntityList::EditorWidgetEntityList(EditorStage& imguiStage) :
		EditorWidget(imguiStage),
		mScene(Scene::GetScene())
	{
		mTitleName = "EntityList";
		mIsWindow = true;
		mWidgetFlags =
			ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_HorizontalScrollbar;
	}

	void EditorWidgetEntityList::Update(F32 deltaTime)
	{
		mHoveredEntity = INVALID_ENTITY;

		if (ImGui::TreeNodeEx("Root", ImGuiTreeNodeFlags_DefaultOpen))
		{
			// 如果drop到了root节点，则设置partent为null
			auto payload = EditorHelper::AcceptDragDropPayload(EditorDragType_Entity);
			if (payload != nullptr)
			{
				ECS::Entity dropEntity = payload->entity;
				if (dropEntity != INVALID_ENTITY && mScene.mTransforms.Contains(dropEntity)) {
					mScene.DetachEntity(dropEntity);
				}
			}

			mRootEntities.clear();
			mEntityAttachedChildrenMap.clear();

			const auto& transforms = mScene.mTransforms.GetEntities();
			for (const auto& transform : transforms)
			{
				auto hierarchy = mScene.mHierarchies.GetComponent(transform);
				if (hierarchy != nullptr && hierarchy->mParent != INVALID_ENTITY) {
					mEntityAttachedChildrenMap[hierarchy->mParent].push_back(transform);
				}
				else {
					mRootEntities.push_back(transform);
				}
			}

			for (const auto& entity : mRootEntities)
			{
				TreeAddEntity(entity);
			}

			ImGui::TreePop();
		}

		UpdateInput();
		UpdateContextMenu();
		UpdateRenamePopup();
	}

	void EditorWidgetEntityList::SetSelectedEntity(ECS::Entity entity)
	{
		EditorHelper::SetSelectedEntity(entity);
		EditorWidgetProperties::SetCurrentEntity(entity);
	}

	void EditorWidgetEntityList::DeleteEntity(ECS::Entity entity)
	{
		mScene.RemoveEntity(entity);
		
		if (EditorHelper::GetSelectedEntity() == entity) {
			EditorHelper::SetSelectedEntity(INVALID_ENTITY);
		}
	}

	void EditorWidgetEntityList::TreeAddEntity(ECS::Entity entity)
	{
		if (entity == INVALID_ENTITY) {
			return;
		}

		bool haveChildren = false;
		auto it = mEntityAttachedChildrenMap.find(entity);
		if (it != mEntityAttachedChildrenMap.end() && it->second.size() > 0) {
			haveChildren = true;
		}

		ImGuiTreeNodeFlags nodeFlags = 
			ImGuiTreeNodeFlags_AllowItemOverlap | 
			ImGuiTreeNodeFlags_SpanAvailWidth;

		nodeFlags |= haveChildren ? ImGuiTreeNodeFlags_OpenOnArrow : ImGuiTreeNodeFlags_Leaf;
		
		if (entity == EditorHelper::GetSelectedEntity()) {
			nodeFlags |= ImGuiTreeNodeFlags_Selected;
		}

		bool isOpen = ImGui::TreeNodeEx(reinterpret_cast<void*>(static_cast<intptr_t>(entity)), nodeFlags, mScene.GetEntityName(entity).c_str());
	
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_RectOnly)) {
			mHoveredEntity = entity;
		}
		
		// check drag drop
		if (ImGui::BeginDragDropSource())
		{
			mDragPayload.entity = entity;
			mDragPayload.type = EditorDragType_Entity;

			EditorHelper::SetDragDropPayload(mDragPayload);
			ImGui::EndDragDropSource();
		}

		auto payload = EditorHelper::AcceptDragDropPayload(EditorDragType_Entity);
		if (payload != nullptr)
		{
			ECS::Entity dropEntity = payload->entity;
			if (dropEntity != INVALID_ENTITY && mScene.mTransforms.Contains(dropEntity))
			{
				if (dropEntity != entity)
				{
					mScene.AttachEntity(dropEntity, entity);
				}
			}
		}

		if (isOpen)
		{
			if (haveChildren)
			{
				for (const auto& childEntity : it->second)
				{
					TreeAddEntity(childEntity);
				}
			}

			ImGui::TreePop();
		}
	}

	void EditorWidgetEntityList::UpdateInput()
	{
		if (!ImGui::IsWindowHovered(
			ImGuiHoveredFlags_AllowWhenBlockedByPopup |
			ImGuiHoveredFlags_AllowWhenBlockedByActiveItem)) {
			return;
		}

		bool isLeftClicked = ImGui::IsMouseClicked(0);
		bool isRightClicked = ImGui::IsMouseClicked(1);

		if (isRightClicked) 
		{
			if (mHoveredEntity != INVALID_ENTITY) {
				SetSelectedEntity(mHoveredEntity);
			}

			ImGui::OpenPopup("##ContextMenu");
		}
		else if (isLeftClicked && mHoveredEntity != INVALID_ENTITY) {
			SetSelectedEntity(mHoveredEntity);
		}
		else if ((isLeftClicked || isRightClicked) && mHoveredEntity == INVALID_ENTITY) {
			SetSelectedEntity(INVALID_ENTITY);
		}
	}

	void EditorWidgetEntityList::UpdateContextMenu()
	{
		if (!ImGui::BeginPopup("##ContextMenu")) {
			return;
		}

		ECS::Entity selectedEntity = EditorHelper::GetSelectedEntity();
		bool hasSelectedEntity = selectedEntity != INVALID_ENTITY;
		if (hasSelectedEntity && ImGui::MenuItem("Copy"))
		{
			mCopiedEntity = selectedEntity;
		}

		if (ImGui::MenuItem("Paste", nullptr, nullptr, mCopiedEntity != INVALID_ENTITY))
		{
			mScene.DuplicateEntity(mCopiedEntity);
		}

		if (hasSelectedEntity && ImGui::MenuItem("Delete"))
		{
			DeleteEntity(selectedEntity);
		}

		if (hasSelectedEntity && ImGui::MenuItem("Rename"))
		{
			mShowRenamePopup = true;
		}

		ImGui::Separator();

		//////////////////////////////////////////////////////////////////////////
		// EMPTY
		if (ImGui::MenuItem("Create Empty"))
		{
			EntityCreateEmpty("Emtpy");
		}

		if (ImGui::BeginMenu("3D Objects"))
		{
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Light"))
		{
			if (ImGui::MenuItem("Directional"))
			{
				EntityCreateDirectionalLight();
			}
			else if (ImGui::MenuItem("Point"))
			{
				EntityCreatePointLight();
			}

			ImGui::EndMenu();
		}

		if (ImGui::MenuItem("Particle"))
		{
			EntityCreateParticle();
		}

		if (ImGui::BeginMenu("Sound"))
		{
			if (ImGui::MenuItem("3D Sound"))
			{
				EntityCreateSound();
			}
			else if (ImGui::MenuItem("Music"))
			{
			}

			ImGui::EndMenu();
		}

		if (ImGui::MenuItem("Terrain"))
		{
			EntityCreateTerrain();
		}

		ImGui::EndPopup();
	}

	void EditorWidgetEntityList::UpdateRenamePopup()
	{
		if (mShowRenamePopup)
		{
			ImGui::OpenPopup("##EntityRename");
			mShowRenamePopup = false;
		}

		if (!ImGui::BeginPopup("##EntityRename")) {
			return;
		}

		auto entity = EditorHelper::GetSelectedEntity();
		if (entity == INVALID_ENTITY)
		{
			ImGui::CloseCurrentPopup();
			ImGui::EndPopup();
			return;
		}

		std::string oldName = mScene.GetEntityName(entity);
		static char name[64] = { 0 };
		sprintf(name, "%s", oldName.c_str());
		ImGui::Text("Name:");
		ImGui::InputText("##edit", name, IM_ARRAYSIZE(name));

		if (name != oldName) {
			mScene.RenameEntity(entity, name);
		}

		if (ImGui::Button("Ok"))
		{
			ImGui::CloseCurrentPopup();
			ImGui::EndPopup();
			return;
		}

		ImGui::EndPopup();
	}

	ECS::Entity EditorWidgetEntityList::EntityCreateEmpty(const std::string& name)
	{
		ECS::Entity entity = mScene.CreateEntityTransform(name);
		if (EditorHelper::GetSelectedEntity() != INVALID_ENTITY) {
			mScene.AttachEntity(entity, EditorHelper::GetSelectedEntity());
		}
		return entity;
	}

	ECS::Entity EditorWidgetEntityList::EntityCreateDirectionalLight()
	{
		ECS::Entity entity = EntityCreateEmpty("Directional Light");
		LightComponent& light = mScene.GetOrCreateLightByEntity(entity);
		light.SetLightType(LightComponent::LightType_Directional);
		return entity;
	}

	ECS::Entity EditorWidgetEntityList::EntityCreatePointLight()
	{
		ECS::Entity entity = EntityCreateEmpty("Point Light");
		LightComponent& light = mScene.GetOrCreateLightByEntity(entity);
		light.SetLightType(LightComponent::LightType_Point);
		return entity;
	}

	ECS::Entity EditorWidgetEntityList::EntityCreateParticle()
	{
		ECS::Entity entity = EntityCreateEmpty("Particle");
		mScene.GetOrCreateParticleByEntity(entity);
		return entity;
	}

	ECS::Entity EditorWidgetEntityList::EntityCreateSound()
	{
		ECS::Entity entity = EntityCreateEmpty("Sound");
		mScene.GetOrCreateSoundByEntity(entity);
		return entity;
	}

	ECS::Entity EditorWidgetEntityList::EntityCreateTerrain()
	{
		ECS::Entity entity = EntityCreateEmpty("Sound");
		TerrainComponent& terrain = mScene.GetOrCreateTerrainByEntity(entity);
		terrain.SetElevation(0);
		return entity;
	}
}