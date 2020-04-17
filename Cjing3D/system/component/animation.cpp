#include "animation.h"
#include "renderer\renderer.h"

namespace Cjing3D
{
	ArmatureComponent::ArmatureComponent() :
		Component(ComponentType_Armature)
	{
	}

	ArmatureComponent::~ArmatureComponent()
	{
	}

	void ArmatureComponent::SetupBoneBuffer()
	{
		GPUBufferDesc desc = {};
		desc.mUsage = USAGE_DYNAMIC;
		desc.mCPUAccessFlags = CPU_ACCESS_WRITE;
		desc.mBindFlags = BIND_SHADER_RESOURCE;
		desc.mMiscFlags = RESOURCE_MISC_BUFFER_STRUCTURED;
		desc.mByteWidth = sizeof(BonePose) * mSkinningBones.size();
		desc.mStructureByteStride = sizeof(BonePose);

		Renderer& renderer = GlobalGetSubSystem<Renderer>();
		const auto result = renderer.GetDevice().CreateBuffer(&desc, mBufferBonePoses, nullptr);
		Debug::ThrowIfFailed(result, "failed to create bone poses buffer:%08x", result);
	}

	bool ArmatureComponent::IsBoneBufferSetup() const
	{
		return mBufferBonePoses.IsValid();
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