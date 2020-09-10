#include "system\sceneSystem.h"

namespace Cjing3D
{
	void SceneSystem::UpdateSceneParticleSystem(Scene& scene, F32 deltaTime)
	{
		ECS::ComponentManager<ParticleComponent>& particles = scene.mParticles;

		for (size_t index = 0; index < particles.GetCount(); index++)
		{
			auto particle = particles[index];
			if (particle != nullptr) {
				particle->UpdateCPU(deltaTime);
			}
		}
	}
}