#include "guiEditorObject.h"
#include "guiEditor.h"
#include "gui\guiEditor\guiEditorInclude.h"
#include "editor\guiEditorMaterial.h"

namespace Cjing3D {
	namespace Editor {



		void InitializeEditorObject(IMGUIStage& imguiStage)
		{
		}

		void ShowObjectAttribute(ObjectComponent* object)
		{
			if (object == nullptr) {
				return;
			}

			Scene& scene = Scene::GetScene();
			ImGui::Separator();
			if (!ImGui::CollapsingHeader("Object", ImGuiTreeNodeFlags_DefaultOpen)) {
				return;
			}

			// mesh 
			auto mesh = scene.mMeshes.GetComponent(object->mMeshID);
			if (mesh != nullptr)
			{
				ImGui::Spacing();
				ImGui::Text("Mesh");

				Entity materialEntity = INVALID_ENTITY;
				if (mesh->mSubsets.size() > 0) {
					// 暂时取第一个subset的materialID
					materialEntity = mesh->mSubsets[0].mMaterialID;
				}

				// material
				static int materialIndex = 1;
				std::string materialNameList = "\0";

				auto& materialManager = scene.mMaterials;
				for (int index = 0; index < materialManager.GetCount(); index++)
				{
					Entity entity = materialManager.GetEntityByIndex(index);
					if (entity == materialEntity) {
						materialIndex = index;
					}

					std::string nodeName = "Entity ";
					auto nameComponent = scene.mNames.GetComponent(entity);
					if (nameComponent != nullptr) {
						nodeName = nodeName + " " + nameComponent->GetString();
					}
					else {
						nodeName = nodeName + std::to_string(entity);
					}

					materialNameList = materialNameList + nodeName + '\0';
				}

				if (ImGui::Combo("Material", &materialIndex, materialNameList.c_str(), 20))
				{
					Entity newMaterial = INVALID_ENTITY;
					if (materialIndex >= 0) {
						newMaterial = materialManager.GetEntityByIndex(materialIndex);
					}

					// 暂时取第一个subset的materialID
					if (mesh->mSubsets.size() > 0) {
						mesh->mSubsets[0].mMaterialID = newMaterial;
					}
				}

				// material
				if (materialEntity != ECS::INVALID_ENTITY) 
				{
					auto material = scene.mMaterials.GetComponent(materialEntity);
					if (material != nullptr) {
						ShowMaterialAttribute(material);
					}
				}
			}
		}
	}
}