#pragma once

#include "gui\guiInclude.h"
#include "utils\tween\tween.h"

namespace Cjing3D {
	namespace Gui {


		template<typename T>
		class SamplerFunc
		{
		public:
			SamplerFunc() {}
			SamplerFunc(F32 duration) :mDuration(duration) {};

			virtual T GetValue(F32 localTime) = 0;
			virtual T GetStartValue() { return GetValue(mDuration); }
			virtual T GetEndValue() { return GetValue(0.0f); }

			F32 GetDuration()const { return mDuration; }

		private:
			F32 mDuration = 0.0f;
		};

		template<typename T>
		using SamplerFuncPtr = std::shared_ptr<SamplerFunc<T>>;


		template<typename T>
		class ConstSamplerFunc : public SamplerFunc<T>
		{
		public:
			ConstSamplerFunc(const T& constValue, F32 duration) : 
				SamplerFunc<T>(duration),
				mConstValue(constValue) {};

			T GetValue(F32 localTime)override
			{
				return mConstValue;
			}

			T GetConstValue()const { return mConstValue; }
			void SetConstValue(const T& constValue) { mConstValue = constValue; }

		private:
			T mConstValue;
		};

		template<typename T, typename EaseT>
		class EaseSamplerFunc : public SamplerFunc<T>
		{
		public:
			constexpr static F32 MinimumFrameDeltaTime = 60.0f;

			EaseSamplerFunc(const T& startValue, const T& endValue, F32 duration, EaseT easing) :
				SamplerFunc<T>(duration),
				mStartValue(startValue),
				mEndValue(endValue)
			{

				mTween = tweeny::tween<T>::from(startValue).to(endValue).during(ConvertTimeToPoint(duration));
				mTween.via(0, easing);
				mTween.seek(0);
			};

			T GetValue(F32 localTime)override
			{
				F32 duration = this->GetDuration();
				return mTween.seek((F32)std::clamp(localTime / duration, 0.0f, 1.0f));
			}

			T GetStartValue()override { return mStartValue; }
			T GetEndValue()override { return mEndValue; }
			U32 ConvertTimeToPoint(F32 time) {
				return U32(std::ceil(time * MinimumFrameDeltaTime));
			}

		private:
			tweeny::tween<T> mTween;
			T mStartValue;
			T mEndValue;
		};

		template<typename T>
		class LoopSampleFunc : public SamplerFunc<T>
		{
		public:
			LoopSampleFunc(const SamplerFunc<T>& sampler, size_t loopTimes) :
				SamplerFunc<T>(sampler.GetDuration() * loopTimes),
				mSampler(sampler),
				mLoopTimes(loopTimes) {}

			T GetValue(F32 localTime)override
			{
				F32 availabelTime = localTime > mSampler.GetDuration() ? localTime % mSampler.GetDuration() : localTime;
				return mSampler.GetValue(availabelTime);
			}

			T GetStartValue()override { return mSampler.GetStartValue(); }
			T GetEndValue()override { return mSampler.GetEndValue(); }

		private:
			SamplerFunc<T> mSampler;
			size_t mLoopTimes;
		};
		
	}
}