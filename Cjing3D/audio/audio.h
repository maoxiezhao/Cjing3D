#pragma once

#include "common\common.h"
#include "core\subSystem.hpp"
#include "audio\audioDevice.h"

namespace Cjing3D
{
	class AudioManager : public SubSystem
	{
	public:
		AudioManager();
		~AudioManager();

		void Initialize(Audio::AudioDevice* device);
		void Uninitialize()override;

		bool LoadSound(const std::string& name, Audio::SoundResource& resource);
		bool LoadSound(const char* data, size_t length, Audio::SoundResource& resource);

		bool CreateInstance(const Audio::SoundResource& resource, Audio::SoundInstance& inst);
		void Play(Audio::SoundInstance& inst);
		void Pause(Audio::SoundInstance& inst);
		void Stop(Audio::SoundInstance& inst);
		void SetVolume(Audio::SoundInstance& inst, F32 volume);
		void SetMasteringVolume(F32 volume);

		void PlayMusic(const std::string& name);
		void PauseMusic();
		void StopMusic();

		void Update3D(Audio::SoundInstance& instance, const Audio::SoundInstance3D& instance3D);

	private:
		Audio::AudioDevice* mAudioDevice = nullptr;
		Audio::SoundInstance mMusicInst;
	};
}