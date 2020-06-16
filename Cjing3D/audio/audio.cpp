#include "audio.h"

namespace Cjing3D
{
	AudioManager::AudioManager(SystemContext& systemContext) :
		SubSystem(systemContext)
	{
	}

	AudioManager::~AudioManager()
	{
	}

	void AudioManager::Initialize(AudioDevice* device)
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

	bool AudioManager::LoadSound(const std::string& name, SoundResource& resource)
	{
		return mAudioDevice->LoadSound(name, resource);
	}

	bool AudioManager::LoadSound(const char* data, size_t length, SoundResource& resource)
	{
		return mAudioDevice->LoadSound(data, length, resource);
	}

	bool AudioManager::CreateInstance(const SoundResource& resource, SoundInstance& inst)
	{
		return mAudioDevice->CreateInstance(resource, inst);
	}

	void AudioManager::Play(SoundInstance& inst)
	{
		mAudioDevice->Play(inst);
	}

	void AudioManager::Pause(SoundInstance& inst)
	{
		mAudioDevice->Pause(inst);
	}

	void AudioManager::Stop(SoundInstance& inst)
	{
		mAudioDevice->Stop(inst);
	}

	void AudioManager::SetVolume(SoundInstance& inst, F32 volume)
	{
		mAudioDevice->SetVolume(inst, volume);
	}

	void AudioManager::SetMasteringVolume(F32 volume)
	{
		mAudioDevice->SetMasteringVolume(volume);
	}

	void AudioManager::PlayMusic(const std::string& name)
	{
	}

	void AudioManager::PauseMusic()
	{
	}
	
	void AudioManager::StopMusic()
	{
	}
}