#pragma once

#include "renderer\passes\renderPass.h"

namespace Cjing3D
{
	class ParticleComponent;
	class MaterialComponent;
	class MeshComponent;

	class ParticlePass : public RenderPass
	{
	public:
		ParticlePass();
		virtual ~ParticlePass();

		virtual void Initialize();
		virtual void Uninitialize();

		void UpdateParticle(ParticleComponent& particle, MeshComponent* mesh = nullptr);
		void DrawParticle(ParticleComponent& particle, MaterialComponent& material);
	};
}