#pragma once

#include "common\common.h"
#include "core\subSystem.hpp"
#include "audio\audioDevice.h"

namespace Cjing3D
{
	class AudioManager : public SubSystem
	{
	public:
		AudioManager(SystemContext& systemContext);
		~AudioManager();

		void Initialize(AudioDevice* device);
		void Uninitialize();

		bool LoadSound(const std::string& name, SoundResource& resource);
		bool LoadSound(const char* data, size_t length, SoundResource& resource);

		bool CreateInstance(const SoundResource& resource, SoundInstance& inst);
		void Play(SoundInstance& inst);
		void Pause(SoundInstance& inst);
		void Stop(SoundInstance& inst);
		void SetVolume(SoundInstance& inst, F32 volume);
		void SetMasteringVolume(F32 volume);

		void PlayMusic(const std::string& name);
		void PauseMusic();
		void StopMusic();

	private:
		AudioDevice* mAudioDevice = nullptr;
		SoundInstance mMusicInst;
	};

}