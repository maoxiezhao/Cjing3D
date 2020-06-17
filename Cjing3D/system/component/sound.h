#pragma once

#include "system\component\componentInclude.h"
#include "audio\audio.h"

namespace Cjing3D {

	class SoundComponent : public Component
	{
	public:
		SoundComponent();
		~SoundComponent();
		
		std::string mFileName;
		SoundResourcePtr mSoundResource;
		Audio::SoundInstance mSoundInstance;

		virtual void Serialize(Archive& archive, U32 seed = 0);
		virtual void Unserialize(Archive& archive)const;

		enum SoundFlag
		{
			EMPTY = 0,
			PLAYING = 1 << 0,
			LOOPED = 1 << 2,
		};
		U32 mSoundFlag = SoundFlag::EMPTY;
		bool mIsDirty = false;
		F32 mVolume = 1.0f;

		inline bool IsDirty() const { return mIsDirty; }
		inline bool IsPlaying()const { return mSoundFlag & SoundFlag::PLAYING; };
		inline bool IsLooped()const { return mSoundFlag & SoundFlag::LOOPED; };

		void Play();
		void Stop();
		void SetLooped(bool isLooped);
		void SetVolume(F32 volume);
	};

}