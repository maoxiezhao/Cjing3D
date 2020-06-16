#include "sound.h"

namespace Cjing3D
{
	SoundComponent::SoundComponent() :
		Component(ComponentType_Sound)
	{
	}

	SoundComponent::~SoundComponent()
	{
	}

	void SoundComponent::Serialize(Archive& archive, U32 seed)
	{
	}

	void SoundComponent::Unserialize(Archive& archive) const
	{
	}
}

