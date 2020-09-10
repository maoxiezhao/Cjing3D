#pragma once

#include "common\common.h"

namespace Cjing3D
{
namespace Audio
{
	enum AUDIO_SUBMIX_TYPE
	{
		AUDIO_SUBMIX_TYPE_SOUNDEFFECT,
		AUDIO_SUBMIX_TYPE_MUSIC,
		AUDIO_SUBMIX_TYPE_USER0,
		AUDIO_SUBMIX_TYPE_USER1,
		AUDIO_SUBMIX_TYPE_COUNT,
	};

	struct SoundResource
	{
		std::shared_ptr<void> mInst = nullptr;
		inline bool IsValid()const { return mInst.get() != nullptr; }
		inline void Clear() { mInst = nullptr; }
	};

	struct SoundInstance
	{
		AUDIO_SUBMIX_TYPE mType = AUDIO_SUBMIX_TYPE_SOUNDEFFECT;
		float mLoopBegin = 0;	// (0 = from beginning)
		float mLoopLength = 0;	// (0 = until the end)

		std::shared_ptr<void> mInst = nullptr;
		inline bool IsValid()const { return mInst.get() != nullptr; }
		inline void Clear() { mInst = nullptr; }
	};

	struct SoundInstance3D
	{
		XMFLOAT3 listenerPos = XMFLOAT3(0, 0, 0);
		XMFLOAT3 listenerUp = XMFLOAT3(0, 1, 0);
		XMFLOAT3 listenerFront = XMFLOAT3(0, 0, 1);
		XMFLOAT3 listenerVelocity = XMFLOAT3(0, 0, 0);
		XMFLOAT3 emitterPos = XMFLOAT3(0, 0, 0);
		XMFLOAT3 emitterUp = XMFLOAT3(0, 1, 0);
		XMFLOAT3 emitterFront = XMFLOAT3(0, 0, 1);
		XMFLOAT3 emitterVelocity = XMFLOAT3(0, 0, 0);
		float emitterRadius = 0;
	};

	class AudioDevice
	{
	public:
		AudioDevice() {};
		~AudioDevice() {};

		virtual void Initialize() = 0;
		virtual void Uninitialize() = 0;

		bool LoadSound(const std::string& name, SoundResource& resource);
		bool LoadSound(const char* data, size_t length, SoundResource& resource);

		virtual bool CreateInstance(const SoundResource& resource, SoundInstance& inst) = 0;
		virtual void Play(SoundInstance& inst) = 0;
		virtual void Pause(SoundInstance& inst) = 0;
		virtual void Stop(SoundInstance& inst) = 0;
		virtual void SetVolume(SoundInstance& inst, F32 volume) = 0;
		virtual void SetMasteringVolume(F32 volume) = 0;

		virtual void Update3D(SoundInstance& instance, const SoundInstance3D& instance3D) = 0;

	protected:
		virtual bool LoadSoundImpl(const char* data, size_t length, SoundResource& resource) = 0;
	};
}
}