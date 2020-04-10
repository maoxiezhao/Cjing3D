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

	AnimationComponent::AnimationComponent() :
		Component(ComponentType_Animation)
	{
	}

	AnimationComponent::~AnimationComponent()
	{
	}

	AnimationComponent::AnimationSampler& AnimationComponent::GetAnimationSamplerByChannel(AnimationChannel& channel)
	{
		Debug::CheckAssertion(channel.mSamplerIndex < mSamplers.size(), "Invalid sampler index in animation channel");
		return mSamplers[channel.mSamplerIndex];
	}

	void AnimationComponent::Serialize(Archive& archive, U32 seed)
	{
	}

	void AnimationComponent::Unserialize(Archive& archive) const
	{
	}

}