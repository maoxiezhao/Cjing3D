#include "guiEditorProperties.h"
#include "guiEditorInclude.h"
#include "guiEditorHelper.h"

namespace Cjing3D
{
	ECS::Entity EditorWidgetProperties::mCurrentEntity = INVALID_ENTITY;

	namespace ComponentProperty
	{
		std::string mComponentContextMenuID = "";
		const F32 mColumn = 100.0f;
		const ImVec2 mSelectButtonSize = ImVec2(100.0f, 0.0f);

		void ShowComponentOptionContextMenu(const std::string& menuID, Component* component, bool canRemove)
		{
			if (component == nullptr) {
				return;
			}

			if (ImGui::BeginPopup(menuID.c_str()))
			{
				if (canRemove)
				{
					if (ImGui::MenuItem("Remove")) {
						Scene::GetScene().RemoveComponentByType(component->GetCurrentEntity(), component->GetType());
					}
				}
				if (ImGui::MenuItem("Copy"))
				{
				}
				if (ImGui::MenuItem("Paste"))
				{
				}
				ImGui::EndPopup();
			};
		}

		bool Begin(const std::string& name, Component* component, bool canRemove)
		{
			auto isOpened = ImGui::CollapsingHeader(name.c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap);
			if (component != nullptr)
			{
				F32 iconWidth = 10.0f;
				ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - iconWidth + 1.0f);
				std::string buttonName = "##" + name;
				if (ImGui::Button(buttonName.c_str(), ImVec2(20.0f, 0.0f)))
				{
					mComponentContextMenuID = name;
					ImGui::OpenPopup(mComponentContextMenuID.c_str());
				}

				if (mComponentContextMenuID == name) {
					ShowComponentOptionContextMenu(mComponentContextMenuID, component, canRemove);
				}
			}

			return isOpened;
		}

		void End()
		{
			ImGui::Separator();
		}
	}

	void EditorWidgetProperties::ShowNameAttribute(ECS::Entity entity)
	{
		if (!ComponentProperty::Begin("Name", nullptr, false)) {
			ComponentProperty::End();
			return;
		}

		std::string oldName = mScene.GetEntityName(entity);
		static char name[64] = { 0 };

		sprintf(name, "%s", oldName.c_str());
		ImGui::InputText("##edit", name, IM_ARRAYSIZE(name));

		if (name != oldName) {
			mScene.RenameEntity(entity, name);
		}

		ComponentProperty::End();
	}

	void EditorWidgetProperties::ShowLayerAttribute(LayerComponent* layer)
	{
		if (!ComponentProperty::Begin("Layout", layer, true)) {
			ComponentProperty::End();
			return;
		}

		U32 mask = layer->GetLayerMask();
		if (ImGui::InputUInt("LayerMask", &mask, 1, 100, ImGuiInputTextFlags_CharsHexadecimal)) {
			Scene::GetScene().SetEntityLayerMask(layer->GetCurrentEntity(), mask);
		}

		ComponentProperty::End();
	}

	void EditorWidgetProperties::ShowTransformAttributes(TransformComponent* transform)
	{
		if (transform == nullptr) {
			return;
		}

		if (!ComponentProperty::Begin("Transform", transform, false)) {
			ComponentProperty::End();
			return;
		}

		XMFLOAT3 translation = transform->GetTranslationLocal();
		float vec3f[3] = { translation.x, translation.y, translation.z };
		ImGui::Text("Position");
		if (ImGui::DragFloat3("", vec3f, 0.005f, -1000, 1000))
		{
			translation.x = vec3f[0];
			translation.y = vec3f[1];
			translation.z = vec3f[2];

			transform->SetTranslationLocal(translation);
		}

		XMFLOAT4 quaternion = transform->GetRotationLocal();
		XMFLOAT3 rotation = QuaternionToRollPitchYaw(quaternion);
		float rVec3f[3] = { rotation.x, rotation.y, rotation.z };
		ImGui::Text("Rotation");
		if (ImGui::DragFloat3("", rVec3f, 0.01f, -XM_2PI, XM_2PI))
		{
			rotation.x = rVec3f[0];
			rotation.y = rVec3f[1];
			rotation.z = rVec3f[2];

			transform->SetRotateFromRollPitchYaw(rotation);
		}

		XMFLOAT3 scale = transform->GetScaleLocal();
		float sVec3f[3] = { scale.x, scale.y, scale.z };
		ImGui::Text("Scale");
		if (ImGui::DragFloat3("", sVec3f, 0.005f, 0, 10))
		{
			scale.x = sVec3f[0];
			scale.y = sVec3f[1];
			scale.z = sVec3f[2];

			transform->SetScaleLocal(scale);
		}

		ComponentProperty::End();
	}

	void EditorWidgetProperties::ShowObjectAttribute(ObjectComponent* object)
	{
		if (object == nullptr) {
			return;
		}

		if (!ComponentProperty::Begin("Object", object, true)) {
			ComponentProperty::End();
			return;
		}

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

		ECS::Entity materialEntity = INVALID_ENTITY;
		auto mesh = mScene.mMeshes.GetComponent(object->mMeshID);
		if (mesh != nullptr)
		{
			// mesh
			ImGui::Text("Mesh");
			ImGui::SameLine(ComponentProperty::mColumn);
			if (ImGui::Button(mScene.GetEntityName(object->mMeshID).c_str(), ComponentProperty::mSelectButtonSize))
			{
				// open mesh selecting popup
			}

			// material
			size_t subsetSize = mesh->mSubsets.size();
			for (int i = 0; i < subsetSize; i++)
			{
				if (subsetSize > 1) {
					ImGui::Text("Material_%d", i);
				}
				else {
					ImGui::Text("Material");
				}

				ImGui::Text("Material");
				ImGui::SameLine(ComponentProperty::mColumn);
				if (ImGui::Button(mScene.GetEntityName(mesh->mSubsets[i].mMaterialID).c_str(), ComponentProperty::mSelectButtonSize))
				{
					// open material selecting popup
				}
			}
			if (subsetSize > 0) {
				materialEntity = mesh->mSubsets[0].mMaterialID;
			}
		}

		ComponentProperty::End();

		// show object material
		if (materialEntity != ECS::INVALID_ENTITY)
		{
			auto material = mScene.mMaterials.GetComponent(materialEntity);
			if (material != nullptr) {
				ShowMaterialAttribute(material);
			}
		}
	}

	void EditorWidgetProperties::ShowMaterialAttribute(MaterialComponent* material)
	{
		if (material == nullptr) {
			return;
		}

		if (!ComponentProperty::Begin("Material", material, false)) {
			ComponentProperty::End();
			return;
		}

		ImGui::Text("BaseColor");
		F32 color[4] = { material->mBaseColor[0], material->mBaseColor[1], material->mBaseColor[2], material->mBaseColor[3] };
		if (ImGui::ColorEdit4("baseColor", color))
		{
			material->mBaseColor[0] = color[0];
			material->mBaseColor[1] = color[1];
			material->mBaseColor[2] = color[2];
			material->mBaseColor[3] = color[3];

			material->SetIsDirty(true);
		}

		std::string baseColorMap = material->mBaseColorMapName;
		ImGui::Text("BaseColorMap");
		if (ImGui::Button(baseColorMap.c_str(), ImVec2(180, 0)))
		{
			auto filePath = EditorHelper::GetFileNameFromOpenFile("Texture File\0*.dds;*.png\0");
			if (!filePath.empty()) {
				material->LoadBaseColorMap(filePath);
			}
		}
		if (material->mBaseColorMap != nullptr)
		{
			if (ImGui::ImageButton(
				material->GetBaseColorMapPtr(),
				ImVec2(64, 64),
				ImVec2(0.0f, 0.0f),
				ImVec2(1.0f, 1.0f),
				0,
				ImVec4(0.0f, 0.0f, 0.0f, 0.0f)))
			{
				auto filePath = EditorHelper::GetFileNameFromOpenFile("Texture File\0*.dds;*.png\0");
				if (!filePath.empty()) {
					material->LoadBaseColorMap(filePath);
				}
			}
		}

		std::string normalMap = material->mNormalMapName;
		ImGui::Text("NormalMapName");
		if (ImGui::Button(normalMap.c_str(), ImVec2(180, 0)))
		{
			auto filePath = EditorHelper::GetFileNameFromOpenFile("Texture File\0*.dds;*.png\0");
			if (!filePath.empty()) {
				material->LoadNormalMap(filePath);
			}
		}

		std::string surfaceMapName = material->mSurfaceMapName;
		ImGui::Text("SurfaceMapName");
		if (ImGui::Button(surfaceMapName.c_str(), ImVec2(180, 0)))
		{
			auto filePath = EditorHelper::GetFileNameFromOpenFile("Texture File\0*.dds;*.png\0");
			if (!filePath.empty()) {
				material->LoadSurfaceMap(filePath);
			}
		}

		bool castingShadow = material->IsCastingShadow();
		if (ImGui::Checkbox("castShadow", &castingShadow)) 
		{
			material->SetCastShadow(castingShadow);
			material->SetIsDirty(true);
		}

		int blendType = static_cast<int>(material->GetBlendMode());
		const char* items[] = { "BlendType_Opaque", "BlendType_Alpha", "BlendType_PreMultiplied" };
		if (ImGui::Combo("BlendMode", &blendType, items, ARRAYSIZE(items))) {
			material->SetBlendMode(static_cast<BlendType>(blendType));
		}

		ComponentProperty::End();
	}

	void EditorWidgetProperties::ShowLightAttribute(LightComponent* light)
	{
		if (!ComponentProperty::Begin("Light", light, true)) {
			ComponentProperty::End();
			return;
		}

		int typeIndex = static_cast<int>(light->GetLightType());
		const char* items[] = { "LightType_Directional", "0LightType_Point", "0LightType_Spot" };
		if (ImGui::Combo("LightType", &typeIndex, items, ARRAYSIZE(items)))
		{
			light->SetLightType(static_cast<LightComponent::LightType>(typeIndex));
		}

		F32 color[3] = { light->mColor[0], light->mColor[1], light->mColor[2] };
		if (ImGui::ColorEdit3("color", color))
		{
			light->mColor[0] = color[0];
			light->mColor[1] = color[1];
			light->mColor[2] = color[2];
		}

		F32 energy = light->mEnergy;
		if (ImGui::DragFloat("energy", &energy, 0.1f, 0.0f, 200.0f))
		{
			light->mEnergy = energy;
		}

		F32 range = light->mRange;
		if (ImGui::DragFloat("range", &range, 0.1f, 0.0f, 200.0f))
		{
			light->mRange = range;
		}

		bool castingShadow = light->IsCastShadow();
		if (ImGui::Checkbox("Shadows", &castingShadow)) {
			light->SetCastShadow(castingShadow);
		}

		F32 shadowBias = light->mShadowBias;
		ImGui::Text("shadowBias");
		ImGui::SameLine(ComponentProperty::mColumn);
		ImGui::PushItemWidth(100); 
		if (ImGui::InputFloat("##shadowBias", &shadowBias, 1.0f, 1.0f, "%.0f"))
		{
			light->mShadowBias = shadowBias;
		}
		ImGui::PopItemWidth();

		ComponentProperty::End();
	}

	void EditorWidgetProperties::ShowSoundAttribute(SoundComponent* sound)
	{
		if (!ComponentProperty::Begin("Sound", sound, true)) {
			ComponentProperty::End();
			return;
		}

		ImGui::Text("Sound File");
		ImGui::SameLine(ComponentProperty::mColumn);
		if (ImGui::Button(sound->mFileName.c_str(), ComponentProperty::mSelectButtonSize))
		{
			auto soundPath = EditorHelper::GetFileNameFromOpenFile("Sound File\0*.wav\0");
			sound->LoadSoundFromFile(soundPath);
		}

		if (sound->IsPlaying())
		{
			if (ImGui::Button("Stop")) {
				sound->Stop();
			}
		}
		else
		{
			if (ImGui::Button("Play")) {
				sound->Play();
			}
		}
		ImGui::SameLine(ComponentProperty::mColumn);
		bool looped = sound->IsLooped();
		if (ImGui::Checkbox("Loop", &looped)) {
			sound->SetLooped(looped);
		}

		I32 volume = (I32)(sound->GetVolue() * 100.0f);
		ImGui::Text("Volume");
		ImGui::SameLine(ComponentProperty::mColumn);
		ImGui::PushItemWidth(100);
		if (ImGui::InputInt("##Volume", &volume, 1, 255)) {
			sound->SetVolume((F32)volume / 100.0f);
		}
		ImGui::PopItemWidth();

		ComponentProperty::End();
	}

	void EditorWidgetProperties::ShowParticleAttribute(ParticleComponent* particle)
	{
		if (!ComponentProperty::Begin("Particle", particle, true)) {
			ComponentProperty::End();
			return;
		}

		ImGui::Text("Pause");
		ImGui::SameLine(ComponentProperty::mColumn);
		bool isPaused = particle->IsPaused();
		if (ImGui::Checkbox("##Pause", &isPaused)) {
			particle->SetPaused(isPaused);
		}

		// mesh
		ImGui::Text("Mesh");
		ImGui::SameLine(ComponentProperty::mColumn);
		if (ImGui::Button(mScene.GetEntityName(particle->mMeshID).c_str(), ComponentProperty::mSelectButtonSize))
		{
			// open mesh selecting popup
		}

		// emit attributes
		I32 maxCount = (I32)particle->GetMaxParticleCount();
		if (ImGui::DragInt("MaxCount", &maxCount, 1.0f, 100, (I32)ParticleComponent::PARTICLE_SUPPORT_MAX_COUNT)) {
			particle->SetMaxParticleCount(maxCount);
		}

		F32 emitPerSec = particle->mEmitCountPerSec;
		if (ImGui::DragFloat("EmitCountPerSec", &emitPerSec, 0.1f, 0.0f, 10000.0f, "%.1f")) {
			particle->mEmitCountPerSec = emitPerSec;
		}

		F32 size = particle->mSize;
		if (ImGui::DragFloat("Size", &size, 0.01f, 0.01f, 100.0f, "%.2f")) {
			particle->mSize = size;
		}

		F32 sizeScaling = particle->mSizeScaling;
		if (ImGui::DragFloat("SizeScaling", &sizeScaling, 0.01f, 0.01f, 100.0f, "%.2f")) {
			particle->mSizeScaling = sizeScaling;
		}

		F32 randomFactor = particle->mRandomFactor;
		if (ImGui::DragFloat("Randomness", &randomFactor, 0.01f, 0.0f, 1.0f, "%.2f")) {
			particle->mRandomFactor = randomFactor;
		}

		F32 life = particle->mLife;
		if (ImGui::DragFloat("Life", &life, 0.01f, 0.0f, 100.0f, "%.2f")) {
			particle->mLife = life;
		}

		F32 lifeRandomness = particle->mRandomLife;
		if (ImGui::DragFloat("LifeRandomness", &lifeRandomness, 0.01f, 0.0f, 1.0f, "%.2f")) {
			particle->mRandomLife = lifeRandomness;
		}

		F32 initialVelocity = particle->mInitialVelocity;
		if (ImGui::DragFloat("InitialVelocity", &initialVelocity, 0.01f, 0.0f, 1000.0f, "%.2f")) {
			particle->mInitialVelocity = initialVelocity;
		}

		F32 rotation = particle->mRotation;
		if (ImGui::DragFloat("Rotation", &rotation, 0.01f, 0.0f, 1000.0f, "%.2f")) {
			particle->mRotation = rotation;
		}

		ImGui::Separator();
		// particle counter
		if (ImGui::CollapsingHeader("Particle Counter", ImGuiTreeNodeFlags_DefaultOpen))
		{
			auto& counter = particle->mParticleCounter;
			ImGui::Text("Particle Counter");
			ImGui::Text("Alive:%d", counter.aliveCount);
			ImGui::Text("Dead:%d", counter.deadCount);
			ImGui::Text("Emit:%d", counter.emitCount);
		}

		ComponentProperty::End();
	}

	EditorWidgetProperties::EditorWidgetProperties(EditorStage& imguiStage) :
		EditorWidget(imguiStage),
		mScene(Scene::GetScene())
	{
		mTitleName = "Properties";
		mIsWindow = true;
		mSize[0] = 500.0f; 
		mWidgetFlags = ImGuiWindowFlags_NoCollapse ;
	}

	void EditorWidgetProperties::Update(F32 deltaTime)
	{
		ImGui::PushItemWidth(200.0f);

		if (mCurrentEntity != INVALID_ENTITY)
		{
			ShowNameAttribute(mCurrentEntity);

			auto transform = mScene.mTransforms.GetComponent(mCurrentEntity);
			if (transform != nullptr) {
				ShowTransformAttributes(transform);
			}
			
			auto layer = mScene.mLayers.GetComponent(mCurrentEntity);
			if (layer != nullptr) {
				ShowLayerAttribute(layer);
			}

			auto object = mScene.mObjects.GetComponent(mCurrentEntity);
			if (object != nullptr) {
				ShowObjectAttribute(object);
			}

			auto material = mScene.mMaterials.GetComponent(mCurrentEntity);
			if (material != nullptr) {
				ShowMaterialAttribute(material);
			}

			auto light = mScene.mLights.GetComponent(mCurrentEntity);
			if (light != nullptr) {
				ShowLightAttribute(light);
			}

			auto sound = mScene.mSounds.GetComponent(mCurrentEntity);
			if (sound != nullptr) {
				ShowSoundAttribute(sound);
			}

			auto particle = mScene.mParticles.GetComponent(mCurrentEntity);
			if (particle != nullptr) {
				ShowParticleAttribute(particle);
			}

			ShowComponentAddButton();
		}
		ImGui::PopItemWidth();
	}

	void EditorWidgetProperties::SetCurrentEntity(ECS::Entity entity)
	{
		mCurrentEntity = entity;
	}

	void EditorWidgetProperties::ShowComponentAddButton()
	{
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
		ImGui::SetCursorPosX(ImGui::GetWindowWidth() * 0.5f - 50);
		if (ImGui::Button("Add Component")) {
			ImGui::OpenPopup("##AddNewComponent");
		}

		if (ImGui::BeginPopup("##AddNewComponent"))
		{
			if (ImGui::BeginMenu("Light"))
			{
				if (ImGui::MenuItem("Directional"))
				{
					auto& light = mScene.GetOrCreateLightByEntity(mCurrentEntity);
					light.SetLightType(LightComponent::LightType_Directional);
				}
				else if (ImGui::MenuItem("Point"))
				{
					auto& light = mScene.GetOrCreateLightByEntity(mCurrentEntity);
					light.SetLightType(LightComponent::LightType_Point);
				}

				ImGui::EndMenu();
			}

			if (ImGui::MenuItem("Particle"))
			{
				mScene.GetOrCreateParticleByEntity(mCurrentEntity);
			}

			if (ImGui::MenuItem("3D Sound"))
			{
				mScene.GetOrCreateSoundByEntity(mCurrentEntity);
			}

			if (ImGui::MenuItem("Terrain"))
			{
				mScene.GetOrCreateTerrainByEntity(mCurrentEntity);
			}

			ImGui::EndPopup();
		}
	}
}