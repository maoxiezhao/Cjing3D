#include "guiEditorParticle.h"
#include "guiEditor.h"
#include "gui\guiEditor\guiEditorInclude.h"
#include "guiEditorScene.h"

namespace Cjing3D {
namespace Editor {


	bool bShowNewParticleWindow = false;
	void ShowNewParticleWindow(F32 deltaTime)
	{

	}

	void ShowParticleAttribute(ParticleComponent* particle)
	{
		if (particle == nullptr) {
			return;
		}

		ImGui::Separator();
		if (!ImGui::CollapsingHeader("Particle", ImGuiTreeNodeFlags_DefaultOpen)) {
			return;
		}

		Scene& scene = Scene::GetScene();
		// emit mesh
		int meshIndex = 0;
		const auto& entityList = scene.mMeshes.GetEntities();
		std::string meshNameList = GetSceneEntityComboList(scene, entityList, particle->mMeshID, meshIndex);
		if (ImGui::Combo("Mesh", &meshIndex, meshNameList.c_str(), 20))
		{
			Entity newEntity = INVALID_ENTITY;
			if (meshIndex > 0) {
				newEntity = entityList[meshIndex - 1];
			}

			// 暂时取第一个subset的materialID
			if (newEntity != particle->mMeshID) {
				particle->mMeshID = newEntity;
			}
		}

		bool isPaused = particle->IsPaused();
		if (ImGui::Checkbox("Pause", &isPaused)) {
			particle->SetPaused(isPaused);
		}

		// particle counter
		ImGui::Separator();
		auto& counter = particle->mParticleCounter;
		ImGui::Text("Particle Counter");
		ImGui::Text("Alive:%d", counter.aliveCount);
		ImGui::Text("Dead:%d",  counter.deadCount);
		ImGui::Text("Emit:%d", counter.emitCount);

		ImGui::Separator();
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
	}

	void ShowNewParticle()
	{
		bShowNewParticleWindow = true;
	}

	void InitializeEditorParticle(IMGUIStage& imguiStage)
	{
		imguiStage.RegisterCustomWindow(ShowNewParticleWindow);
	}
}
}