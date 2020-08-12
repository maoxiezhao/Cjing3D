#include "guiEditorHierarchy.h"
#include "guiEditor.h"
#include "gui\guiEditor\guiEditorInclude.h"

#include "editor\guiEditorLight.h"
#include "editor\guiEditorObject.h"
#include "editor\guiEditorSound.h"
#include "editor\guiEditorMaterial.h"

namespace Cjing3D {
	namespace Editor {

		namespace {
			static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

			static ECS::Entity currentEntity = ECS::INVALID_ENTITY;
		}

		bool ShowEntityList(Scene& scene, Entity entity, U32 currentIndex, I32 selectionIndex, I32& nodeClicked)
		{
			std::string nodeName = "Entity ";
			auto nameComponent = scene.mNames.GetComponent(entity);
			if (nameComponent != nullptr) {
				nodeName = nodeName + " " + nameComponent->GetString();
			}
			else {
				nodeName = nodeName + std::to_string(entity);
			}

			// 通过mask检测是否点击到
			ImGuiTreeNodeFlags node_flags = base_flags | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
			const bool is_selected = (selectionIndex == currentIndex);
			if (is_selected) {
				node_flags |= ImGuiTreeNodeFlags_Selected;
			}

			ImGui::TreeNodeEx((void*)(intptr_t)currentIndex, node_flags, nodeName.c_str());
			if (ImGui::IsItemClicked()) {
				nodeClicked = currentIndex;
			}

			return is_selected;
		};

		void ShowSoundEntityList(Scene& scene)
		{
			static int slectionIndex = -1;
			int nodeClicked = -1;
			auto& soundManager = scene.mSounds;
			for (int index = 0; index < soundManager.GetCount(); index++)
			{
				Entity entity = soundManager.GetEntityByIndex(index);
				if (ShowEntityList(scene, entity, index, slectionIndex, nodeClicked)) {
					currentEntity = entity;
				}
			}

			if (nodeClicked != -1) {
				slectionIndex = nodeClicked;
			}
		}

		void ShowMaterialEntityList(Scene& scene)
		{
			static int slectionIndex = -1;
			int nodeClicked = -1;
			auto& materialManager = scene.mMaterials;
			for (int index = 0; index < materialManager.GetCount(); index++)
			{
				Entity entity = materialManager.GetEntityByIndex(index);
				if (ShowEntityList(scene, entity, index, slectionIndex, nodeClicked)) {
					currentEntity = entity;
				}
			}

			if (nodeClicked != -1) {
				slectionIndex = nodeClicked;
			}
		}

		void ShowObjectEntityList(Scene& scene)
		{
			static int slectionIndex = -1;
			int nodeClicked = -1;
			auto& objectManager = scene.mObjects;
			for (int index = 0; index < objectManager.GetCount(); index++)
			{
				Entity entity = objectManager.GetEntityByIndex(index);
				if (ShowEntityList(scene, entity, index, slectionIndex, nodeClicked)) {
					currentEntity = entity;
				}
			}

			if (nodeClicked != -1) {
				slectionIndex = nodeClicked;
			}
		}

		using TransformChildrenMap = std::map<ECS::Entity, std::vector<ECS::Entity>>;
		bool processTransformNode(Scene& scene, Entity entity, int currentIndex, std::set<ECS::Entity>& transformSet, TransformChildrenMap& transformChildren) {
			if (transformSet.find(entity) != transformSet.end()) {
				return false;
			}
			transformSet.insert(entity);

			std::string nodeName = "Entity ";
			auto nameComponent = scene.mNames.GetComponent(entity);
			if (nameComponent != nullptr) {
				nodeName = nodeName + " " + nameComponent->GetString();
			}
			else {
				nodeName = nodeName + std::to_string(entity);
			}

			// 通过mask检测是否点击到
			ImGuiTreeNodeFlags node_flags = base_flags;
			const bool is_selected = (currentEntity == entity);
			if (is_selected) {
				node_flags |= ImGuiTreeNodeFlags_Selected;
			}

			// has children
			auto childrenIt = transformChildren.find(entity);
			bool hasChildren = (childrenIt != transformChildren.end());
			if (!hasChildren) {
				node_flags |= ImGuiTreeNodeFlags_Leaf;
			}

			bool isChildClicked = false;
			if (ImGui::TreeNodeEx((void*)(intptr_t)currentIndex, node_flags, nodeName.c_str()))
			{
				if (hasChildren)
				{
					U32 childIndex = 0;
					for (ECS::Entity child : childrenIt->second) {
						isChildClicked = isChildClicked || processTransformNode(scene, child, childIndex++, transformSet, transformChildren);
					}
				}
				ImGui::TreePop();
			}

			if (!isChildClicked && ImGui::IsItemClicked()) {
				currentEntity = entity;
				isChildClicked = true;
			}
			return isChildClicked;
		};

		void ShowHierarchyList(Scene& scene)
		{
			static std::set<ECS::Entity> transformSet;
			transformSet.clear();

			auto& transforms = scene.mTransforms;
			std::vector<ECS::Entity> rootTransforms;
			TransformChildrenMap transformChildrenMap;
			// 临时计算每个transform children，性能存在问题，仅用于in-engine editor
			for (int index = 0; index < transforms.GetCount(); index++)
			{
				Entity entity = transforms.GetEntityByIndex(index);
				auto hierarchy = scene.mHierarchies.GetComponent(entity);
				if (hierarchy == nullptr || hierarchy->mParent == ECS::INVALID_ENTITY) {
					rootTransforms.push_back(entity);
					continue;
				}

				auto findIt = transformChildrenMap.find(hierarchy->mParent);
				if (findIt == transformChildrenMap.end())
				{
					std::vector<ECS::Entity> children;
					children.push_back(entity);

					transformChildrenMap[hierarchy->mParent] = children;
				}
				else
				{
					findIt->second.push_back(entity);
				}
			}

			U32 rootIndex = 0;
			for (ECS::Entity entity : rootTransforms) {
				processTransformNode(scene, entity, rootIndex++, transformSet, transformChildrenMap);
			}
		}

		void ShowTransformAttributes(TransformComponent* transform)
		{
			if (transform == nullptr) {
				return;
			}

			ImGui::Separator();
			if (!ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
				return;
			}

			XMFLOAT3 translation = transform->GetTranslationLocal();
			float vec3f[3] = { translation.x, translation.y, translation.z };
			if (ImGui::DragFloat3("position", vec3f, 0.005f, -1000, 1000))
			{
				translation.x = vec3f[0];
				translation.y = vec3f[1];
				translation.z = vec3f[2];

				transform->SetTranslationLocal(translation);
			}

			XMFLOAT4 quaternion = transform->GetRotationLocal();
			XMFLOAT3 rotation = QuaternionToRollPitchYaw(quaternion);
			float rVec3f[3] = { rotation.x, rotation.y, rotation.z };
			if (ImGui::DragFloat3("rotation", rVec3f, 0.01f, -XM_2PI, XM_2PI))
			{
				rotation.x = rVec3f[0];
				rotation.y = rVec3f[1];
				rotation.z = rVec3f[2];

				transform->SetRotateFromRollPitchYaw(rotation);
			}

			XMFLOAT3 scale = transform->GetScaleLocal();
			float sVec3f[3] = { scale.x, scale.y, scale.z };
			if (ImGui::DragFloat3("scale", sVec3f, 0.005f, 0, 10))
			{
				scale.x = sVec3f[0];
				scale.y = sVec3f[1];
				scale.z = sVec3f[2];

				transform->SetScaleLocal(scale);
			}
		}

		void ShowNameComponentAttribute(ECS::Entity entity)
		{
			ImGui::Separator();

			Scene& scene = Scene::GetScene();
			auto name = scene.mNames.GetComponent(currentEntity);
			if (name != nullptr) 
			{
				const std::string nameStr = std::string("Name:") + name->GetString();
				ImGui::Text(nameStr.c_str());
			}
			else
			{	
				const std::string nameStr = std::string("Name:NULL");
				ImGui::Text(nameStr.c_str());
			}

			// rename box
			ImGui::SameLine();
			if (ImGui::Button("Rename")) {
				ImGui::OpenPopup("Rename box");
			}

			const std::string entityStr = std::string("Entity:") + std::to_string(entity);
			ImGui::Text(entityStr.c_str());

			if (ImGui::BeginPopupModal("Rename box", NULL))
			{
				ImGui::SetNextItemWidth(100.0f);
				static char nameStr[128] = "";
				ImGui::InputText("name", nameStr, IM_ARRAYSIZE(nameStr));

				if (ImGui::Button("Confirm"))
				{
					scene.RenameEntity(entity, nameStr);
					ImGui::CloseCurrentPopup();
				}
				ImGui::SameLine();
				if (ImGui::Button("Close"))
					ImGui::CloseCurrentPopup();
				ImGui::EndPopup();
			}
		}

		void ShowLayerComponentAttribute(LayerComponent* layer)
		{
			ImGui::Separator();
			if (!ImGui::CollapsingHeader("Layer")) {
				return;
			}

			U32 mask = layer->GetLayerMask();
			if (ImGui::InputUInt("LayerMask", &mask, 1, 100, ImGuiInputTextFlags_CharsHexadecimal)) {
				Scene::GetScene().SetEntityLayerMask(layer->GetCurrentEntity(), mask);
			}
		}

		void showEntityListWindow(F32 deltaTime)
		{
			ImGui::SetNextWindowPos(ImVec2(5, 30), ImGuiCond_Once);
			ImGui::SetNextWindowSize(ImVec2(280, 740), ImGuiCond_Once);
			ImGui::Begin("Entity Window");

			Scene& scene = Scene::GetScene();
			ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
			if (ImGui::BeginTabBar("EntityBar", tab_bar_flags))
			{
				if (ImGui::BeginTabItem("Hierarchy"))
				{
					ShowHierarchyList(scene);
					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("Objects"))
				{
					ShowObjectEntityList(scene);
					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("Materials"))
				{
					ShowMaterialEntityList(scene);
					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("Sounds"))
				{
					ShowSoundEntityList(scene);
					ImGui::EndTabItem();
				}
				ImGui::EndTabBar();
			}
			ImGui::End();
		}

		void ShowEntityAttribute(F32 deltaTime)
		{
			if (currentEntity == INVALID_ENTITY) return;

			// show render window
			ImGui::SetNextWindowPos(ImVec2(1170, 30), ImGuiCond_Once);
			ImGui::SetNextWindowSize(ImVec2(260, 740), ImGuiCond_Once);
			ImGui::Begin("Attribute Window");

			Scene& scene = Scene::GetScene();

			// name
			ShowNameComponentAttribute(currentEntity);

			// transform
			auto transform = scene.mTransforms.GetComponent(currentEntity);
			if (transform != nullptr) {
				ShowTransformAttributes(transform);
			}

			// layer
			auto layer = scene.mLayers.GetComponent(currentEntity);
			if (layer != nullptr) {
				ShowLayerComponentAttribute(layer);
			}
		
			// object
			auto object = scene.mObjects.GetComponent(currentEntity);
			if (object != nullptr) {
				ShowObjectAttribute(object);
			}

			// light 
			auto light = scene.mLights.GetComponent(currentEntity);
			if (light != nullptr) {
				ShowLightAttribute(light);
			}

			// sound
			auto sound = scene.mSounds.GetComponent(currentEntity);
			if (sound != nullptr) {
				ShowSoundAttribute(sound);
			}

			// material
			auto material = scene.mMaterials.GetComponent(currentEntity);
			if (material != nullptr) {
				ShowMaterialAttribute(material);
			}

			ImGui::End();
		}


		void InitializeEditorHierarchy(IMGUIStage& imguiStage)
		{
			imguiStage.RegisterCustomWindow(showEntityListWindow);
			imguiStage.RegisterCustomWindow(ShowEntityAttribute);
		}

	}
}