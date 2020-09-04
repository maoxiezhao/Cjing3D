#include "audio.h"
#include "core\globalContext.hpp"
#include "resource\resourceManager.h"

namespace Cjing3D
{
	SoundResourcePtr currentSoundResource = nullptr;

	AudioManager::AudioManager()
	{
	}

	AudioManager::~AudioManager()
	{
	}

	void AudioManager::Initialize(Audio::AudioDevice* device)
	{
		Debug::CheckAssertion(device != nullptr, "Invalid aduio device.");
		mAudioDevice = device;
		mAudioDevice->Initialize();
	}

	void AudioManager::Uninitialize()
	{
		if (mAudioDevice != nullptr)
		{
			mAudioDevice->Uninitialize();
			CJING_MEM_DELETE(mAudioDevice);
		}
	}

	bool AudioManager::LoadSound(const std::string& name, Audio::SoundResource& resource)
	{
		return mAudioDevice->LoadSound(name, resource);
	}

	bool AudioManager::LoadSound(const char* data, size_t length, Audio::SoundResource& resource)
	{
		return mAudioDevice->LoadSound(data, length, resource);
	}

	bool AudioManager::CreateInstance(const Audio::SoundResource& resource, Audio::SoundInstance& inst)
	{
		return mAudioDevice->CreateInstance(resource, inst);
	}

	void AudioManager::Play(Audio::SoundInstance& inst)
	{
		mAudioDevice->Play(inst);
	}

	void AudioManager::Pause(Audio::SoundInstance& inst)
	{
		mAudioDevice->Pause(inst);
	}

	void AudioManager::Stop(Audio::SoundInstance& inst)
	{
		mAudioDevice->Stop(inst);
	}

	void AudioManager::SetVolume(Audio::SoundInstance& inst, F32 volume)
	{
		mAudioDevice->SetVolume(inst, volume);
	}

	void AudioManager::SetMasteringVolume(F32 volume)
	{
		mAudioDevice->SetMasteringVolume(volume);
	}

	void AudioManager::PlayMusic(const std::string& name)
	{
		auto soundResource = GetGlobalContext().gResourceManager->GetOrCreate<Cjing3D::SoundResource>(name);
		if (soundResource != currentSoundResource)
		{
			currentSoundResource = soundResource;
			CreateInstance(soundResource->mSound, mMusicInst);
			Play(mMusicInst);
		}
	}

	void AudioManager::PauseMusic()
	{
		Pause(mMusicInst);
	}

	void AudioManager::StopMusic()
	{
		Stop(mMusicInst);
	}

	void AudioManager::Update3D(Audio::SoundInstance& instance, const Audio::SoundInstance3D& instance3D)
	{
		mAudioDevice->Update3D(instance, instance3D);
	}
}