#pragma once

#include "gui\guiAnimation\guiAnimation.h"
#include "gui\guiAnimation\valueAnimation_impl.h"

#include "utils\signal\connectionList.h"

namespace Cjing3D {
	namespace Gui {

		template<typename T>
		class ValueChanelAnimation;

		//////////////////////////////////////////////////////////////////////////////
		// 动画采样输出值
		template<typename T>
		class SamplerOutputValue
		{
		public:
			SamplerOutputValue() = default;
			SamplerOutputValue(const T& value) : mValue(value) {}
			SamplerOutputValue(const SamplerOutputValue<T>& rhs) = delete;
			SamplerOutputValue& operator=(const SamplerOutputValue<T>& rhs) = delete;

			SamplerOutputValue(SamplerOutputValue<T>&& rhs)
			{
				mValue = std::move(rhs.mValue);
				mControl = std::move(rhs.mControl);

				if (mControl != nullptr) {
					mControl->Connect(this);
				}
			}

			SamplerOutputValue& operator=(SamplerOutputValue&& rhs)
			{
				mValue = std::move(rhs.mValue);
				mControl = std::move(rhs.mControl);

				if (mControl != nullptr) {
					mControl->Connect(this);
				}
			}

			const T& Value()const { return mValue; }
			T& Value() { return mValue; }
			const T* ValuePtr()const { return &mValue; }
			T* ValuePtr() { return &mValue; }

			void Connect(ValueChanelAnimation<T>* control) { mControl = control; }
			void Disconnect()
			{
				if (mControl != nullptr)
				{
					mControl->Disconnect();
					mControl = nullptr;
				}
			}
			bool IsConnected()const { return mControl != nullptr; }

		private:
			T mValue;
			ValueChanelAnimation<T>* mControl = nullptr;
		};

		//////////////////////////////////////////////////////////////////////////////
		// 指定值的采样序列
		template<typename T>
		class ValueSamplerSequence
		{
		public:
			ValueSamplerSequence() = delete;
			ValueSamplerSequence(const T& value) :
				mStartValue(value){}

			template<template <typename, typename> class SamplerFuncT, typename EaseT>
			void AddSampler(const T& endValue, F32 duration, EaseT easing)
			{
				std::shared_ptr<SamplerFuncT<T, EaseT>> sampler = std::make_shared<SamplerFuncT<T, EaseT>>(GetEndValue(), endValue, duration, easing);
				mSamplers.push_back(sampler);
				mDuration += duration;
			}

			void AddSampler(const SamplerFuncPtr<T>& func);
			void AddSampler(const ValueSamplerSequence<T>& queue);
			F32 GetDuration()const { return mDuration; }

			T GetValue(F32 localtime);
			T GetStartValue() { return mStartValue; }
			T GetEndValue() { return mSamplers.empty() ? mStartValue : mSamplers.back()->GetEndValue(); }

		private:
			std::vector<SamplerFuncPtr<T>> mSamplers;
			T mStartValue;
			F32 mDuration = 0.0f;
		};

		template<typename T>
		inline void ValueSamplerSequence<T>::AddSampler(const SamplerFuncPtr<T>& func)
		{
			mSamplers.push_back(func);
			mDuration += func.GetDuration();
		}

		template<typename T>
		inline void ValueSamplerSequence<T>::AddSampler(const ValueSamplerSequence<T>& queue)
		{
			mSamplers.insert(mSamplers.end(), queue.mSamplers.begin, queue.mSamplers.end());
			mDuration += queue.GetDuration();
		}

		template<typename T>
		inline T ValueSamplerSequence<T>::GetValue(F32 localtime)
		{
			if (localtime <= 0.0f) 
			{
				return GetStartValue();
			}
			else if (localtime >= GetDuration()) 
			{
				return GetEndValue();
			}
			else
			{
				for (auto& sampler : mSamplers) 
				{
					F32 samplerDuration = sampler->GetDuration();
					if (localtime < samplerDuration) {
						return sampler->GetValue(localtime);
					}

					localtime -= samplerDuration;
				}
			}
		}


		//////////////////////////////////////////////////////////////////////////////
		// 改变指定值的动画
		template<typename T>
		class ValueChanelAnimation : public AnimationBase
		{
		public:
			ValueChanelAnimation() = delete;
			ValueChanelAnimation(T* valuePtr) : 
				mValuePtr(valuePtr),
				mSequence(*valuePtr) {}

			ValueChanelAnimation(SamplerOutputValue<T>* output) :
				mValuePtr(output->ValuePtr()),
				mOutput(output),
				mSequence(output->Value())
			{
				output->Disconnect();
				output->Connect(this);
			}
			~ValueChanelAnimation()
			{
				Disconnect();
			}

			F32 GetDuration()const override { return mSequence.GetDuration(); };
			T* GetValuePtr() { return mValuePtr; }
			T GetValue()const { return *mValuePtr; }
			ValueSamplerSequence<T>& GetSequence() { return mSequence; }
			void Connect(SamplerOutputValue<T>* output);
			void Disconnect();

			Signal<void(const T & oldValue, const T & newValue)> OnValueChanged;

		protected:
			void UpdateImpl(F32 delta)override;

		private:
			T* mValuePtr = nullptr;
			SamplerOutputValue<T>* mOutput = nullptr;
			ValueSamplerSequence<T> mSequence;
		};

		template<typename T>
		inline void ValueChanelAnimation<T>::Connect(SamplerOutputValue<T>* output)
		{
			mOutput = output;
		}

		template<typename T>
		inline void ValueChanelAnimation<T>::Disconnect()
		{
			if (mOutput != nullptr) {
				mOutput = nullptr;
			}
			Stop();
		}

		template<typename T>
		inline void ValueChanelAnimation<T>::UpdateImpl(F32 delta)
		{
			F32 localTime = GetLocalTime();
			T newValue = mSequence.GetValue(localTime);
			if (newValue != *mValuePtr)
			{
				T oldValue = *mValuePtr;
				*mValuePtr = newValue;
				OnValueChanged(oldValue, newValue);
			}
		}

}
}