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

		const auto result = Renderer::GetDevice().CreateBuffer(&desc, mBufferBonePoses, nullptr);
		Debug::ThrowIfFailed(result, "failed to create bone poses buffer:%08x", result);
	}

	bool ArmatureComponent::IsBoneBufferSetup() const
	{
		return mBufferBonePoses.IsValid();
	}

	void ArmatureComponent::Serialize(Archive& archive, U32 seed)
	{
		archive >> mRootBone;
		archive >> mInverseBindMatrices;

		U32 boneSize = 0;
		archive >> boneSize;
		if (boneSize > 0)
		{
			mSkinningBones.resize(boneSize);
			for (int i = 0; i < boneSize; i++)
			{
				mSkinningBones[i] = SerializeEntity(archive, seed);
			}
		}

		U32 bonePoseSize = 0;
		archive >> bonePoseSize;
		if (bonePoseSize > 0)
		{
			mBonePoses.resize(bonePoseSize);
			for (int i = 0; i < bonePoseSize; i++)
			{
				archive >> mBonePoses[i].bonePose0;
				archive >> mBonePoses[i].bonePose1;
				archive >> mBonePoses[i].bonePose2;
			}
		}
	}

	void ArmatureComponent::Unserialize(Archive& archive) const
	{
		archive << mRootBone;
		archive << mInverseBindMatrices;

		archive << mSkinningBones.size();
		for (ECS::Entity entity : mSkinningBones)
		{
			archive << entity;
		}

		archive << mBonePoses.size();
		for (const auto& bonePose : mBonePoses)
		{
			archive << bonePose.bonePose0;
			archive << bonePose.bonePose1;
			archive << bonePose.bonePose2;
		}
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
		archive >> mTimeStart;
		archive >> mTimeEnd;
		archive >> mCurrentTimer;
		archive >> mFlag;

		U32 channelSize = 0;
		archive >> channelSize;
		if (channelSize > 0)
		{
			mChannels.resize(channelSize);
			for (int i = 0; i < channelSize; i++)
			{
				archive >> mChannels[i].mTargetPath;
				mChannels[i].mTargetNode = SerializeEntity(archive, seed);
				archive >> mChannels[i].mSamplerIndex;
			}
		}

		U32 samplerSize = 0;
		archive >> samplerSize;
		if (samplerSize > 0)
		{
			mSamplers.resize(samplerSize);
			for (int i = 0; i < samplerSize; i++)
			{
				archive >> mSamplers[i].mSamplerMode;
				archive >> mSamplers[i].mKeyframeTimes;
				archive >> mSamplers[i].mKeyframeDatas;
			}
		}
	}

	void AnimationComponent::Unserialize(Archive& archive) const
	{
		archive << mTimeStart;
		archive << mTimeEnd ;
		archive << mCurrentTimer;
		archive << mFlag;

		archive << mChannels.size();
		for (const auto& channel : mChannels)
		{
			archive << channel.mTargetPath;
			archive << channel.mTargetNode;
			archive << channel.mSamplerIndex;
		}

		archive << mSamplers.size();
		for (const auto& sampler : mSamplers)
		{
			archive << sampler.mSamplerMode;
			archive << sampler.mKeyframeTimes;
			archive << sampler.mKeyframeDatas;
		}
	}

}