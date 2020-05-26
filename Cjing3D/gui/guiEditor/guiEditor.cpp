#include "guiEditor.h"
#include "guiEditorInclude.h"

#include "guiEditorLight.h"

namespace Cjing3D {
namespace Editor {

	static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
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

	void ShowNameComponentAttribute(NameComponent* name)
	{
		if (name == nullptr) {
			return;
		}

		const std::string nameStr = std::string("Name:") + name->GetString();
		ImGui::Text(nameStr.c_str());
	
		const std::string entityStr = std::string("Entity:") + std::to_string(name->GetCurrentEntity());
		ImGui::Text(entityStr.c_str());
	}

	void ShowTransformAttribute(TransformComponent* transform)
	{
		if (transform == nullptr) {
			return;
		}

		ImGui::Text("Transform");

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

	Entity currentObject = INVALID_ENTITY;
	void ShowObjectAttribute(F32 deltaTime)
	{
		if (currentObject == INVALID_ENTITY) return;

		Scene& scene = Scene::GetScene();
		auto object = scene.mObjects.GetComponent(currentObject);
		if (object == nullptr) {
			return;
		}

		ImGui::SetNextWindowPos(ImVec2(1070, 20), ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(350, 360), ImGuiCond_Once);
		ImGui::Begin("Object attributes");

		// transform
		auto transform = scene.mTransforms.GetComponent(currentObject);
		ShowTransformAttribute(transform);

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
		}

		ImGui::End();
	}

	Entity currentMaterial = INVALID_ENTITY;
	void ShowMaterialAttribute(F32 deltaTime)
	{
		if (currentMaterial == INVALID_ENTITY) return;

		ImGui::SetNextWindowPos(ImVec2(1070, 20), ImGuiCond_Once);
		ImGui::SetNextWindowSize(ImVec2(350, 360), ImGuiCond_Once);
		ImGui::Begin("Material attributes");

		Scene& scene = Scene::GetScene();
		auto material = scene.mMaterials.GetComponent(currentMaterial);
		if (material != nullptr) {
			ImGui::Text("BaseColor");

			F32 color[4] = { material->mBaseColor[0], material->mBaseColor[1], material->mBaseColor[2], material->mBaseColor[3] };
			if (ImGui::ColorEdit4("color", color))
			{
				material->mBaseColor[0] = color[0];
				material->mBaseColor[1] = color[1];
				material->mBaseColor[2] = color[2];
				material->mBaseColor[3] = color[3];

				material->SetIsDirty(true);
			}

			bool castingShadow = material->IsCastingShadow();
			if (ImGui::Checkbox("castShadow", &castingShadow)) {
				material->SetCastShadow(castingShadow);

				material->SetIsDirty(true);
			}
		}

		ImGui::End();
	}

	Entity currentTerrain = INVALID_ENTITY;
	void ShowTerrainAttribute(F32 deltaTime)
	{
		if (currentTerrain == INVALID_ENTITY) return;

		ImGui::SetNextWindowPos(ImVec2(1070, 20), ImGuiCond_Once);
		ImGui::SetNextWindowSize(ImVec2(350, 360), ImGuiCond_Once);
		ImGui::Begin("Terrain attributes");

		Scene& scene = Scene::GetScene();
		auto transform = scene.mTransforms.GetComponent(currentTerrain);
		ShowTransformAttribute(transform);

		auto terrain = scene.mTerrains.GetComponent(currentTerrain);
		if (terrain != nullptr)
		{
			I32 elevation = (I32)terrain->GetElevation();
			if (ImGui::SliderInt("Elevation", &elevation, 0, 200)) {
				terrain->SetElevation((U32)elevation);
			}
		}

		ImGui::End();
	}

	bool showAnimationWindow = false;
	void ShowAnimationAttribute(F32 deltaTime)
	{
		if (showAnimationWindow == false) return;

		ImGui::SetNextWindowPos(ImVec2(1070, 20), ImGuiCond_Once);
		ImGui::SetNextWindowSize(ImVec2(350, 160), ImGuiCond_Once);
		ImGui::Begin("Animations attributes", &showAnimationWindow);

		Scene& scene = Scene::GetScene();
		auto& animations = scene.mAnimations;
		if (animations.GetCount() <= 0) {
			ImGui::End();
			return;
		}

		// animation list
		static int animationIndex = 0;
		std::string animationNameList = "\0";

		for (int index = 0; index < animations.GetCount(); index++)
		{
			Entity entity = animations.GetEntityByIndex(index);
			std::string nodeName = "Entity ";
			auto nameComponent = scene.mNames.GetComponent(entity);
			if (nameComponent != nullptr) {
				nodeName = nodeName + " " + nameComponent->GetString();
			}
			else {
				nodeName = nodeName + std::to_string(entity);
			}

			animationNameList = animationNameList + nodeName + '\0';
		}

		ImGui::Combo("Animations", &animationIndex, animationNameList.c_str(), 20);

		if (animationIndex >= 0 && animationIndex < animations.GetCount()) {
			auto animation = animations.GetComponentByIndex(animationIndex);
			if (animation != nullptr)
			{
				bool looped = animation->IsLooped();
				if (ImGui::Checkbox("Loop", &looped)) {
					animation->SetIsLooped(looped);
				}
				ImGui::SameLine(100);
				if (ImGui::Button("Play")) {
					if (animation->IsPlaying()) 
					{
						animation->Pause();
					}
					else
					{
						animation->Play();
					}
				}
				ImGui::SameLine(150);
				if (ImGui::Button("Stop")) {
					animation->Stop();
				}
			}
		}



		ImGui::End();
	}

	void ShowRenderAttribute()
	{
		SystemContext& systemContext = SystemContext::GetSystemContext();
		Renderer& renderer = systemContext.GetSubSystem<Renderer>();

		RenderPath* renderPath = renderer.GetRenderPath();
		if (renderPath == nullptr) return;

		// tone mapping exposure
		RenderPath3D* renderPath3D = dynamic_cast<RenderPath3D*>(renderPath);
		if (renderPath3D != nullptr)
		{
			F32 explore = renderPath3D->GetExposure();
			ImGui::SetNextItemWidth(100);
			if (ImGui::DragFloat("ToneMappingExposure", &explore, 0.1f, 0.1f, 10.0f)) {
				renderPath3D->SetExposure(explore);
			}

			bool fxaaEnable = renderPath3D->IsFXAAEnable();
			if (ImGui::Checkbox("FXAA", &fxaaEnable)) {
				renderPath3D->SetFXAAEnable(fxaaEnable);
			}
		}

		// ambient light
		F32x3 ambientColor = renderer.GetAmbientColor();
		F32* ambientColors = ambientColor.data();
		if (ImGui::ColorEdit3("color", ambientColors))
		{
			renderer.SetAmbientColor(ambientColor);
		}
	}

	bool showRenderWindow = false;
	void ShowRenderProperties(F32 deltaTime)
	{
		if (showRenderWindow == false) return;

		// show render window
		ImGui::SetNextWindowPos(ImVec2(1070, 20), ImGuiCond_Once);
		ImGui::SetNextWindowSize(ImVec2(350, 160), ImGuiCond_Once);
		ImGui::Begin("Render Window", &showRenderWindow);

		if (ImGui::BeginTabBar("Properties", ImGuiTabBarFlags_None))
		{
			if (ImGui::BeginTabItem("Renderer"))
			{
				ShowRenderAttribute();
				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}

		ImGui::End();
	}

	static int currentTransformEntity = ECS::INVALID_ENTITY;
	void ShowHierarchyWindow(F32 deltaTime)
	{
		if (currentTransformEntity == INVALID_ENTITY) return;

		// show render window
		ImGui::SetNextWindowPos(ImVec2(1070, 20), ImGuiCond_Once);
		ImGui::SetNextWindowSize(ImVec2(350, 160), ImGuiCond_Once);
		ImGui::Begin("Hierarchy Window", &showRenderWindow);

		// transform
		auto transform = Scene::GetScene().mTransforms.GetComponent(currentTransformEntity);
		ShowTransformAttribute(transform);

		ImGui::End();
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
		const bool is_selected = (currentTransformEntity == entity);
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
			currentTransformEntity = entity;
			isChildClicked = true;
		}

		return isChildClicked;
	};

	void ShowHierarchyList()
	{
		static std::set<ECS::Entity> transformSet;
		transformSet.clear();

		Scene& scene = Scene::GetScene();
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
		for(ECS::Entity entity : rootTransforms)
		{
			processTransformNode(scene, entity, rootIndex++, transformSet, transformChildrenMap);
		}
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////

	void showEntityListWindow(F32 deltaTime)
	{
		ImGui::SetNextWindowPos(ImVec2(10, 350), ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(350, 300), ImGuiCond_Always);
		ImGui::Begin("Entity Window");

		Scene& scene = Scene::GetScene();
		ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
		if (ImGui::BeginTabBar("EntityBar", tab_bar_flags))
		{
			currentObject = INVALID_ENTITY;
			SetCurrentLight(INVALID_ENTITY);

			// show object window
			if (ImGui::BeginTabItem("Objects"))
			{
				static int objectSelectionIndex = -1;
				int nodeClicked = -1;
				auto& objectManager = scene.mObjects;
				for (int index = 0; index < objectManager.GetCount(); index++)
				{
					Entity entity = objectManager.GetEntityByIndex(index);
					if (ShowEntityList(scene, entity, index, objectSelectionIndex, nodeClicked)) {
						currentObject = entity;
					}
				}

				if (nodeClicked != -1) {
					objectSelectionIndex = nodeClicked;
				}

				ImGui::EndTabItem();
			}

			// show hierarchy list
			if (ImGui::BeginTabItem("Hierarchy"))
			{
				ShowHierarchyList();
				ImGui::EndTabItem();
			}

			// show light window
			if (ImGui::BeginTabItem("Lights"))
			{
				static int lightSelectionIndex = -1;
				int nodeClicked = -1;
				auto& lightManager = scene.mLights;
				for (int index = 0; index < lightManager.GetCount(); index++)
				{
					Entity entity = lightManager.GetEntityByIndex(index);
					if (ShowEntityList(scene, entity, index, lightSelectionIndex, nodeClicked)) {
						SetCurrentLight(entity);
					}
				}

				if (nodeClicked != -1) {
					lightSelectionIndex = nodeClicked;
				}

				ImGui::EndTabItem();
			}

			// show material window
			if (ImGui::BeginTabItem("Materials"))
			{
				static int materialSelectionIndex = -1;
				int nodeClicked = -1;
				auto& materialManager = scene.mMaterials;
				for (int index = 0; index < materialManager.GetCount(); index++)
				{
					Entity entity = materialManager.GetEntityByIndex(index);
					if (ShowEntityList(scene, entity, index, materialSelectionIndex, nodeClicked)) {
						currentMaterial = entity;
					}
				}

				if (nodeClicked != -1) {
					materialSelectionIndex = nodeClicked;
				}

				ImGui::EndTabItem();
			}

			// show material window
			if (ImGui::BeginTabItem("Terrains"))
			{
				static int terrainSelectionIndex = -1;
				int nodeClicked = -1;
				auto& terrainManager = scene.mTerrains;
				for (int index = 0; index < terrainManager.GetCount(); index++)
				{
					Entity entity = terrainManager.GetEntityByIndex(index);
					if (ShowEntityList(scene, entity, index, terrainSelectionIndex, nodeClicked)) {
						currentTerrain = entity;
					}
				}

				if (nodeClicked != -1) {
					terrainSelectionIndex = nodeClicked;
				}

				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}
		ImGui::End();
	}

	void ShowBasicWindow(F32 deltaTime)
	{
		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar;
		bool isOpen = false;

		ImGui::SetNextWindowPos(ImVec2(10, 20), ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(350, 320), ImGuiCond_Always);
		ImGui::Begin("Application info", &isOpen, windowFlags);

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("Base"))
			{
				if (ImGui::MenuItem("Animations")) { showAnimationWindow = true; }
				if (ImGui::MenuItem("Properties")) { showRenderWindow = true; }
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Component"))
			{
				if (ImGui::MenuItem("Light")) { ShowNewLightWindow(); }
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		ImGui::Text("Cjing3D v0.0.1");
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", deltaTime, 1.0f / deltaTime);
		ImGui::Text("F4-Show Debug; F5-Load scene;");
		ImGui::Text("F6-Save scene; F7-Clear scene;");
		ImGui::Text("WASD-Move camera;  RB-Rotate camera");
		ImGui::Text("");
		ImGui::Text(Profiler::GetInstance().GetProfileString().c_str());
		ImGui::Text("");
		ImGui::Text("heap memory usage:%.3f KB", Memory::GetMemUsage() / 1000.0f);
		ImGui::End();
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////

	void InitializeEditor(IMGUIStage& imguiStage)
	{
#ifndef _DISBALE_IMGUI_EDITOR_

		imguiStage.RegisterCustomWindow(ShowBasicWindow);
		imguiStage.RegisterCustomWindow(showEntityListWindow);
		imguiStage.RegisterCustomWindow(ShowObjectAttribute);
		imguiStage.RegisterCustomWindow(ShowMaterialAttribute);
		imguiStage.RegisterCustomWindow(ShowTerrainAttribute);
		imguiStage.RegisterCustomWindow(ShowRenderProperties);
		imguiStage.RegisterCustomWindow(ShowAnimationAttribute);
		imguiStage.RegisterCustomWindow(ShowHierarchyWindow);



		InitializeEditorLight(imguiStage);
#endif
	}
}
}

