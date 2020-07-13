#include "sound.h"
#include "resource\resourceManager.h"

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
		archive >> mFileName;
		archive >> mSoundFlag;
		archive >> mVolume;

		mSoundResource = GlobalGetSubSystem<ResourceManager>().GetOrCreate<SoundResource>(mFileName);

		if (mSoundResource->mSound.IsValid()) {
			GlobalGetSubSystem<Audio::AudioManager>().CreateInstance(mSoundResource->mSound, mSoundInstance);
		}
	}

	void SoundComponent::Unserialize(Archive& archive) const
	{
		archive << mFileName;
		archive << mSoundFlag;
		archive << mVolume;
	}

	void SoundComponent::LoadSoundFromFile(const std::string& filePath)
	{	
		mFileName = filePath;
		mSoundResource = GlobalGetSubSystem<ResourceManager>().GetOrCreate<SoundResource>(filePath);

		if (mSoundResource->mSound.IsValid()) {
			GlobalGetSubSystem<Audio::AudioManager>().CreateInstance(mSoundResource->mSound, mSoundInstance);
		}
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

	F32 SoundComponent::GetVolue() const
	{
		return mVolume;;
	}
}

