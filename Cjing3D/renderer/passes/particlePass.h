#pragma once

#include "renderer\passes\renderPass.h"

namespace Cjing3D
{
	class ParticleComponent;
	class MaterialComponent;

	class ParticlePass : public RenderPass
	{
	public:
		ParticlePass();
		virtual ~ParticlePass();

		virtual void Initialize();
		virtual void Uninitialize();

		void UpdateParticle(ParticleComponent& particle);
		void DrawParticle(ParticleComponent& particle, MaterialComponent& material);
		PipelineState& GetPipelineState();
	};
}