#pragma once

#include "audio\audioDevice.h"

#ifdef _WIN32

#include <xaudio2.h>
#include <xaudio2fx.h>
#include <x3daudio.h>
#pragma comment(lib,"xaudio2.lib")

#endif

namespace Cjing3D
{
namespace Audio
{
	class AudioDeviceX : public AudioDevice
	{
	public:
		AudioDeviceX();
		~AudioDeviceX();

		virtual void Initialize();
		virtual void Uninitialize();

		virtual bool CreateInstance(const SoundResource& resource, SoundInstance& inst);
		virtual void Play(SoundInstance& inst);
		virtual void Pause(SoundInstance& inst);
		virtual void Stop(SoundInstance& inst);
		virtual void SetVolume(SoundInstance& inst, F32 volume);
		virtual void SetMasteringVolume(F32 volume);

		virtual void Update3D(SoundInstance& instance, const SoundInstance3D& instance3D);

	protected:
		virtual bool LoadSoundImpl(const char* data, size_t length, SoundResource& resource);

	private:
		ComPtr<IXAudio2> mAudioDevice = nullptr;
		X3DAUDIO_HANDLE mAudio3D = {};
		//	Master voice（主声音）：将音频数据最后写到咱们的音频输出设备上
		//	Submix voice（混合声音）：也就是将多个源声音进行混合
		IXAudio2MasteringVoice* mMasteringVoice = nullptr;
		IXAudio2SubmixVoice* mSubmixVoices[AUDIO_SUBMIX_TYPE_COUNT] = {};
		XAUDIO2_VOICE_DETAILS mMasteringVoiceDetails = {};
		IXAudio2SubmixVoice* reverbSubmix = nullptr;
	};
}
}