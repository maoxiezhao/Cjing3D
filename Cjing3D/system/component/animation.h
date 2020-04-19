#pragma once

#include "system\component\componentInclude.h"

namespace Cjing3D {
	  
	// 骨骼节点
	class ArmatureComponent : public Component
	{
	public:
		ArmatureComponent();
		~ArmatureComponent();

		ECS::Entity mRootBone = ECS::INVALID_ENTITY;
		std::vector<ECS::Entity> mSkinningBones;
		std::vector<XMFLOAT4X4> mInverseBindMatrices;

		struct BonePose
		{
			XMFLOAT4 bonePose0;
			XMFLOAT4 bonePose1;
			XMFLOAT4 bonePose2;

			inline void Store(const XMMATRIX& m)
			{
				XMFLOAT4X4 mat;
				XMStoreFloat4x4(&mat, m);

				bonePose0 = XMFLOAT4(mat._11, mat._21, mat._31, mat._41);
				bonePose1 = XMFLOAT4(mat._12, mat._22, mat._32, mat._42);
				bonePose2 = XMFLOAT4(mat._13, mat._23, mat._33, mat._43);
			}

			inline XMMATRIX Load()const
			{
				return XMMATRIX(
					bonePose0.x, bonePose1.x, bonePose2.x, 0,
					bonePose0.y, bonePose1.y, bonePose2.y, 0,
					bonePose0.z, bonePose1.z, bonePose2.z, 0,
					bonePose0.w, bonePose1.w, bonePose2.w, 1
				);
			}
		};
		std::vector<BonePose> mBonePoses;
		GPUBuffer mBufferBonePoses;

		AABB mAABB;

		void SetupBoneBuffer();
		bool IsBoneBufferSetup()const;

		virtual void Serialize(Archive& archive, U32 seed = 0);
		virtual void Unserialize(Archive& archive)const;
	};

	class AnimationComponent : public Component
	{
	public:
		AnimationComponent();
		~AnimationComponent();

		// AnimationChannel表示一个node的transform的一个属性的变化
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

		// 表示一个属性在一系列关键帧下的值
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