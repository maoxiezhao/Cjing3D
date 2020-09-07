#pragma once

#include "guiEditorWidget.h"
#include "system\sceneSystem.h"

namespace Cjing3D
{
	class EditorWidgetProperties : public EditorWidget
	{
	public:
		EditorWidgetProperties(EditorStage& imguiStage);

		void Update(F32 deltaTime)override;
		static void SetCurrentEntity(ECS::Entity entity);

	private:
		void ShowNameAttribute(ECS::Entity entity);
		void ShowLayerAttribute(LayerComponent* layer);
		void ShowTransformAttributes(TransformComponent* transform);
		void ShowObjectAttribute(ObjectComponent* object);
		void ShowMaterialAttribute(MaterialComponent* material);
		void ShowLightAttribute(LightComponent* light);
		void ShowSoundAttribute(SoundComponent* sound);
		void ShowParticleAttribute(ParticleComponent* particle);
		void ShowComponentAddButton();

	private:
		Scene& mScene;
		static ECS::Entity mCurrentEntity;
	};
}