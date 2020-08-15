#include "audio.h"
#include "resource\resourceManager.h"

namespace Cjing3D
{
namespace Audio
{
	SoundResourcePtr currentSoundResource = nullptr;

	AudioManager::AudioManager(GlobalContext& globalContext) :
		SubSystem(globalContext)
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
		auto& resourceManager = GlobalGetSubSystem<ResourceManager>();
		auto soundResource = resourceManager.GetOrCreate<Cjing3D::SoundResource>(name);
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

	void AudioManager::Update3D(SoundInstance& instance, const SoundInstance3D& instance3D)
	{
		mAudioDevice->Update3D(instance, instance3D);
	}
}
}