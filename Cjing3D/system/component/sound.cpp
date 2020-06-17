#include "sound.h"

namespace Cjing3D
{
	SoundComponent::SoundComponent() :
		Component(ComponentType_Sound)
	{
	}

	SoundComponent::~SoundComponent()
	{
		mSoundResource = nullptr;
		mSoundInstance.Clear();
	}

	void SoundComponent::Serialize(Archive& archive, U32 seed)
	{
	}

	void SoundComponent::Unserialize(Archive& archive) const
	{
	}

	void SoundComponent::Play()
	{
		if (!IsPlaying())
		{
			mSoundFlag |= SoundFlag::PLAYING;
			mIsDirty = true;
		}
	}

	void SoundComponent::Stop()
	{
		if (IsPlaying()) 
		{
			mSoundFlag &= ~SoundFlag::PLAYING;
			mIsDirty = true;
		}
	}

	void SoundComponent::SetLooped(bool isLooped)
	{
		if (isLooped)
		{
			mSoundFlag |= SoundFlag::LOOPED;
		}
		else
		{
			mSoundFlag &= ~SoundFlag::LOOPED;
		}
	}

	void SoundComponent::SetVolume(F32 volume)
	{
		mVolume = volume;
		mIsDirty = true;
	}
}

