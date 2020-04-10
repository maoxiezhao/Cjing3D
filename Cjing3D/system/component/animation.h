#pragma once

#include "system\component\componentInclude.h"

namespace Cjing3D {
	  
	class ArmatureComponent : public Component
	{
	public:
		ArmatureComponent();
		~ArmatureComponent();

		std::vector<ECS::Entity> mSkiningBones;
		std::vector<XMFLOAT4X4> mInverseBindMatrices;

		virtual void Serialize(Archive& archive, U32 seed = 0);
		virtual void Unserialize(Archive& archive)const;
	};

	class AnimationComponent : public Component
	{
	public:
		AnimationComponent();
		~AnimationComponent();

		struct AnimationChannel
		{
			enum TargetPath
			{
				TargetPath_Unknown,
				TargetPath_Scale,
				TargetPath_Rotation,
				TargetPath_Translation
			};
			TargetPath mTargetPath;
			ECS::Entity mTargetNode;
			U32 mSamplerIndex = 0;
		};

		struct AnimationSampler
		{
			enum SamplerMode
			{
				SamplerMode_Linear,
				SamplerMode_Step,
			};
			SamplerMode mSamplerMode;
			std::vector<F32> mKeyframeTimes;
			std::vector<F32> mKeyframeDatas;
		};

		std::vector<AnimationChannel> mChannels;
		std::vector<AnimationSampler> mSamplers;

		F32 mTimeStart = 0;
		F32 mTimeEnd = 0;
		F32 mCurrentTimer = 0;

		enum Flag
		{
			Flag_Empty = 0,
			Flag_Playing = 1,
			Flag_Looped = 1 << 1
		};
		U32 mFlag = Flag_Empty;

		bool IsPlaying() const { return mFlag & Flag_Playing; }
		bool IsLooped() const { return mFlag & Flag_Looped; }
		void Play() { mFlag |= Flag_Playing; }
		void Pause() { mFlag &= ~Flag_Playing; }
		void Stop() { mFlag &= ~Flag_Playing; mCurrentTimer = 0.0f; }
		void SetIsLooped(bool isLooped) { isLooped ? mFlag |= Flag_Looped : mFlag &= ~Flag_Looped;}
		void Restart() { Play(); mCurrentTimer = 0.0f; }

		AnimationSampler& GetAnimationSamplerByChannel(AnimationChannel& channel);

		virtual void Serialize(Archive& archive, U32 seed = 0);
		virtual void Unserialize(Archive& archive)const;
	};

}