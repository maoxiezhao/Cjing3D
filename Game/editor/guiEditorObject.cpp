#include "guiEditorObject.h"
#include "guiEditor.h"
#include "gui\guiEditor\guiEditorInclude.h"
#include "editor\guiEditorMaterial.h"
#include "editor\guiEditorScene.h"

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

			// object
			{
				int typeIndex = static_cast<int>(object->GetRenderableType());
				const char* items[] = { "RenderableType_Opaque", "RenderableType_Transparent" };
				if (ImGui::Combo("RenderType", &typeIndex, items, ARRAYSIZE(items)))
				{
					object->SetRenderableType(static_cast<RenderableType>(typeIndex));
				}

				bool isRenderable = object->IsRenderable();
				if (ImGui::Checkbox("isRenderable", &isRenderable)) {
					object->SetRenderable(isRenderable);
				}
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
				static int materialIndex = 0;
				const auto& entityList = scene.mMaterials.GetEntities();
				std::string materialNameList = GetSceneEntityComboList(scene, entityList, materialEntity, materialIndex);
				if (ImGui::Combo("Material", &materialIndex, materialNameList.c_str(), 20))
				{
					Entity newMaterial = INVALID_ENTITY;
					if (materialIndex > 0) {
						newMaterial = entityList[materialIndex - 1];
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