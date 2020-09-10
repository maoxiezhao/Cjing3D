#include "audioDeviceX.h"
#include "utils\container\dynamicArray.h"

#include <vector>

#define fourccRIFF 'FFIR'
#define fourccDATA 'atad'
#define fourccFMT ' tmf'
#define fourccWAVE 'EVAW'
#define fourccXWMA 'AMWX'
#define fourccDPDS 'sdpd'

// TODO：流式加载支持

namespace Cjing3D
{
namespace Audio
{
	namespace
	{
		struct SoundResourceImpl
		{
			WAVEFORMATEX mWfx = {};
			Container::DynamicArray<U8> mAudioData;
		};

		struct SoundInstantceImpl
		{
			IXAudio2SourceVoice* mSourceVoice = nullptr;
			XAUDIO2_BUFFER mBuffer = {};

			XAUDIO2_VOICE_DETAILS mVoiceDetails = {};
			Container::DynamicArray<float> mOutputMatrix;
			Container::DynamicArray<float> mChannelAzimuths;

			~SoundInstantceImpl()
			{
				if (mSourceVoice != nullptr) {
					mSourceVoice->Stop();
					mSourceVoice->DestroyVoice();
				}

				mOutputMatrix.clear();
				mChannelAzimuths.clear();
			}
		};

		// 从当前wavedata中获取指定类型chunk
		bool FindChunk(const U8* data, DWORD fourcc, DWORD& dwChunkSize, DWORD& dwChunkDataPosition)
		{
			// chunk data
			DWORD dwChunkType;
			DWORD dwChunkDataSize;
			DWORD dwRIFFDataSize = 0;
			DWORD bytesRead = 0;
			DWORD dwOffset = 0;

			const U8* dataPos = data;
			while (true)
			{
				Memory::Memcpy(&dwChunkType, (void*)(dataPos), sizeof(DWORD));
				dataPos += sizeof(DWORD);
				Memory::Memcpy(&dwChunkDataSize, (void*)(dataPos), sizeof(DWORD));
				dataPos += sizeof(DWORD);

				switch (dwChunkType)
				{
				case fourccRIFF:
					dwRIFFDataSize = dwChunkDataSize;
					dwChunkDataSize = 4;
					dataPos += sizeof(DWORD); // skip dwFileType
					break;

				default:
					dataPos += dwChunkDataSize;
				}

				// skip dwChunkType,dwChunkDataSize
				dwOffset += sizeof(DWORD) * 2;

				if (dwChunkType == fourcc)
				{
					dwChunkSize = dwChunkDataSize;
					dwChunkDataPosition = dwOffset;
					return true;
				}

				dwOffset += dwChunkDataSize;

				if (bytesRead >= dwRIFFDataSize) return false;
			}
			return true;
		}
	}

	AudioDeviceX::AudioDeviceX()
	{
	}

	AudioDeviceX::~AudioDeviceX()
	{
	}

	void AudioDeviceX::Initialize()
	{
		Logger::Info("Initialize audio device:xaudio2.");

		// initialize base
		HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
		Debug::ThrowIfFailed(hr, "[Audio] Failed to initialize xaudio.");

		hr = XAudio2Create(&mAudioDevice, 0, XAUDIO2_DEFAULT_PROCESSOR);
		Debug::ThrowIfFailed(hr, "[Audio] Failed to initialize xaudio.");

#ifdef CJING_DEBUG
		XAUDIO2_DEBUG_CONFIGURATION debugConfig = {};
		debugConfig.TraceMask = XAUDIO2_LOG_ERRORS | XAUDIO2_LOG_WARNINGS;
		debugConfig.BreakMask = XAUDIO2_LOG_ERRORS | XAUDIO2_LOG_WARNINGS;
		mAudioDevice->SetDebugConfiguration(&debugConfig);
#endif
		// create matering voice
		hr = mAudioDevice->CreateMasteringVoice(&mMasteringVoice);
		Debug::ThrowIfFailed(hr, "[Audio] Failed to initialize xaudio.");

		// create submix voice
		mMasteringVoice->GetVoiceDetails(&mMasteringVoiceDetails);
		for (int i = 0; i < AUDIO_SUBMIX_TYPE_COUNT; i++)
		{
			hr = mAudioDevice->CreateSubmixVoice(
				&mSubmixVoices[i],
				mMasteringVoiceDetails.InputChannels,
				mMasteringVoiceDetails.InputSampleRate,
				0, 0, 0, 0
			);
			Debug::ThrowIfFailed(hr, "[Audio] Failed to create submix voice.");
		}

		// sets all global 3D audio constants.
		DWORD channelMask;
		mMasteringVoice->GetChannelMask(&channelMask);
		hr = X3DAudioInitialize(channelMask, X3DAUDIO_SPEED_OF_SOUND, mAudio3D);
	}

	void AudioDeviceX::Uninitialize()
	{
		for (int i = 0; i < AUDIO_SUBMIX_TYPE_COUNT; i++)
		{
			if (mSubmixVoices[i] != nullptr) {
				mSubmixVoices[i]->DestroyVoice();
			}
		}

		if (mMasteringVoice != nullptr) {
			mMasteringVoice->DestroyVoice();
		}

		mAudioDevice->StopEngine();

		CoUninitialize();
	}

	bool AudioDeviceX::LoadSoundImpl(const char* data, size_t length, SoundResource& resource)
	{
		auto soundInst = CJING_MAKE_SHARED<SoundResourceImpl>();
		resource.mInst = soundInst;

		DWORD dwChunkSize;
		DWORD dwChunkPosition;
		const U8* audioData = reinterpret_cast<const U8*>(data);

		// check file type
		if (!FindChunk(audioData, fourccRIFF, dwChunkSize, dwChunkPosition))
		{
			Debug::Warning("Faild to open sound resource");
			return false;
		}
		DWORD filetype;
		memcpy(&filetype, audioData + dwChunkPosition, sizeof(DWORD));
		if (filetype != fourccWAVE)
		{
			Debug::Warning("Faild to open sound resource: invalid file type.");
			return false;
		}

		// write WAVEFORMAT
		if (!FindChunk(audioData, fourccFMT, dwChunkSize, dwChunkPosition))
		{
			Debug::Warning("Faild to open sound resource");
			return false;
		}
		memcpy(&soundInst->mWfx, audioData + dwChunkPosition, dwChunkSize);
		soundInst->mWfx.wFormatTag = WAVE_FORMAT_PCM;

		// write audio data
		if (!FindChunk(audioData, fourccDATA, dwChunkSize, dwChunkPosition))
		{
			Debug::Warning("Faild to open sound resource");
			return false;
		}
		soundInst->mAudioData.resize(dwChunkSize);
		memcpy(soundInst->mAudioData.data(), audioData + dwChunkPosition, dwChunkSize);

		return true;
	}

	bool AudioDeviceX::CreateInstance(const SoundResource& resource, SoundInstance& inst)
	{
		const auto& resourceInst = std::static_pointer_cast<SoundResourceImpl>(resource.mInst);
		auto soundInst = CJING_MAKE_SHARED<SoundInstantceImpl>();
		inst.mInst = soundInst;

		// setup source voice
		XAUDIO2_SEND_DESCRIPTOR sfxSends[] = {
			{ XAUDIO2_SEND_USEFILTER, mSubmixVoices[static_cast<int>(inst.mType)] },
		};
		XAUDIO2_VOICE_SENDS sfxSendLists = { ARRAYSIZE(sfxSends), sfxSends };

		HRESULT hr = mAudioDevice->CreateSourceVoice(
			&soundInst->mSourceVoice,
			&resourceInst->mWfx,
			0,
			XAUDIO2_DEFAULT_FREQ_RATIO,
			nullptr,
			&sfxSendLists,
			nullptr
		);
		if (FAILED(hr))
		{
			Debug::Warning("Failed to create sound instance.");
			return false;
		}

		// get details
		soundInst->mSourceVoice->GetVoiceDetails(&soundInst->mVoiceDetails);
		soundInst->mOutputMatrix.resize(soundInst->mVoiceDetails.InputChannels * mMasteringVoiceDetails.InputChannels);
		// The table values must be within 0.0f to X3DAUDIO_2PI
		soundInst->mChannelAzimuths.resize(soundInst->mVoiceDetails.InputChannels);
		for (size_t i = 0; i < soundInst->mVoiceDetails.InputChannels; ++i)
		{
			soundInst->mChannelAzimuths[i] = X3DAUDIO_2PI * F32(i) / F32(soundInst->mVoiceDetails.InputChannels);
		}

		// initialize buffer
		auto& soundBuffer = soundInst->mBuffer;
		soundBuffer = {};
		soundBuffer.AudioBytes = (U32)resourceInst->mAudioData.size();
		soundBuffer.pAudioData = resourceInst->mAudioData.data();
		soundBuffer.Flags = XAUDIO2_END_OF_STREAM;
		soundBuffer.LoopCount = XAUDIO2_LOOP_INFINITE;
		soundBuffer.LoopBegin = U32(inst.mLoopBegin * mMasteringVoiceDetails.InputSampleRate);
		soundBuffer.LoopLength = U32(inst.mLoopLength * mMasteringVoiceDetails.InputSampleRate);

		hr = soundInst->mSourceVoice->SubmitSourceBuffer(&soundBuffer);
		if (FAILED(hr))
		{
			Debug::Warning("Failed to submit sound source buffer.");
			return false;
		}

		return true;
	}

	void AudioDeviceX::Play(SoundInstance& inst)
	{
		if (inst.IsValid())
		{
			auto soundInst = std::static_pointer_cast<SoundInstantceImpl>(inst.mInst);
			HRESULT hr = soundInst->mSourceVoice->Start();
			Debug::ThrowIfFailed(hr, "Failed to play sound");
		}
	}

	void AudioDeviceX::Pause(SoundInstance& inst)
	{
		if (inst.IsValid())
		{
			auto soundInst = std::static_pointer_cast<SoundInstantceImpl>(inst.mInst);
			soundInst->mSourceVoice->Stop();
		}
	}

	void AudioDeviceX::Stop(SoundInstance& inst)
	{
		if (inst.IsValid())
		{
			auto soundInst = std::static_pointer_cast<SoundInstantceImpl>(inst.mInst);
			soundInst->mSourceVoice->Stop();
			// flush and submit buffer
			soundInst->mSourceVoice->FlushSourceBuffers();
			soundInst->mSourceVoice->SubmitSourceBuffer(&soundInst->mBuffer);
		}
	}

	void AudioDeviceX::SetVolume(SoundInstance& inst, F32 volume)
	{
		if (inst.IsValid())
		{
			auto soundInst = std::static_pointer_cast<SoundInstantceImpl>(inst.mInst);
			soundInst->mSourceVoice->SetVolume(volume);
		}
	}

	void AudioDeviceX::SetMasteringVolume(F32 volume)
	{
		mMasteringVoice->SetVolume(volume);
	}

	void AudioDeviceX::Update3D(SoundInstance& instance, const SoundInstance3D& instance3D)
	{
		if (instance.IsValid())
		{
			auto soundInst = std::static_pointer_cast<SoundInstantceImpl>(instance.mInst);

			// listener config
			X3DAUDIO_LISTENER listener = {};
			listener.Position    = instance3D.listenerPos;
			listener.OrientFront = instance3D.listenerFront;
			listener.OrientTop   = instance3D.listenerUp;
			listener.Velocity    = instance3D.listenerVelocity;

			// emitter config
			X3DAUDIO_EMITTER emitter = {};
			emitter.Position = instance3D.emitterPos;
			emitter.OrientFront = instance3D.emitterFront;
			emitter.OrientTop = instance3D.emitterUp;
			emitter.Velocity = instance3D.emitterVelocity;
			emitter.InnerRadius = instance3D.emitterRadius;
			emitter.InnerRadiusAngle = X3DAUDIO_PI / 4.0f;
			emitter.ChannelCount = soundInst->mVoiceDetails.InputChannels;
			emitter.pChannelAzimuths = soundInst->mChannelAzimuths.data();
			emitter.ChannelRadius = 0.1f;
			emitter.CurveDistanceScaler = 1;
			emitter.DopplerScaler = 1;

			UINT32 flags = 0;
			flags |= X3DAUDIO_CALCULATE_MATRIX;
			flags |= X3DAUDIO_CALCULATE_LPF_DIRECT;
			flags |= X3DAUDIO_CALCULATE_DOPPLER;
			flags |= X3DAUDIO_CALCULATE_REVERB;
			flags |= X3DAUDIO_CALCULATE_LPF_REVERB;

			X3DAUDIO_DSP_SETTINGS settings = {};
			settings.SrcChannelCount = soundInst->mVoiceDetails.InputChannels;
			settings.DstChannelCount = mMasteringVoiceDetails.InputChannels;
			settings.pMatrixCoefficients = soundInst->mOutputMatrix.data();

			// Calculates DSP settings with respect to 3D parameters.
			X3DAudioCalculate(mAudio3D, &listener, &emitter, flags, &settings);

			HRESULT hr = soundInst->mSourceVoice->SetFrequencyRatio(settings.DopplerFactor);
			if (FAILED(hr))
			{
				Debug::Warning("Failed to SetFrequencyRatio.");
				return;
			}

			hr = soundInst->mSourceVoice->SetOutputMatrix(
				mSubmixVoices[instance.mType],
				settings.SrcChannelCount,
				settings.DstChannelCount,
				settings.pMatrixCoefficients
			);
			if (FAILED(hr))
			{
				Debug::Warning("Failed to SetOutputMatrix.");
				return;
			}

			XAUDIO2_FILTER_PARAMETERS FilterParametersDirect = { LowPassFilter, 2.0f * sinf(X3DAUDIO_PI / 6.0f * settings.LPFDirectCoefficient), 1.0f };
			hr = soundInst->mSourceVoice->SetOutputFilterParameters(mSubmixVoices[instance.mType], &FilterParametersDirect);
			if (FAILED(hr))
			{
				Debug::Warning("Failed to SetOutputFilterParameters.");
				return;
			}
		}
	}
}
}
