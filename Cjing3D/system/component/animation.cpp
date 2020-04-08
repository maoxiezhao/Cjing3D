#include "animation.h"

namespace Cjing3D
{
	ArmatureComponent::ArmatureComponent() :
		Component(ComponentType_Armature)
	{
	}

	ArmatureComponent::~ArmatureComponent()
	{
	}

	void ArmatureComponent::Serialize(Archive& archive, U32 seed)
	{
	}

	void ArmatureComponent::Unserialize(Archive& archive) const
	{
	}

	Animation::Animation() :
		Component(ComponentType_Animation)
	{
	}

	Animation::~Animation()
	{
	}

	void Animation::Serialize(Archive& archive, U32 seed)
	{
	}

	void Animation::Unserialize(Archive& archive) const
	{
	}

}