#include "particle.h"

namespace Cjing3D {

	ParticleComponent::ParticleComponent() :
		Component(ComponentType_Particle)
	{
	}

	ParticleComponent::~ParticleComponent()
	{
	}

	void ParticleComponent::Serialize(Archive& archive, U32 seed)
	{
	}

	void ParticleComponent::Unserialize(Archive& archive) const
	{
	}

}