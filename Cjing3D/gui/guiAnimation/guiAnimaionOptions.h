#pragma once

#include "gui\guiAnimation\guiAnimation.h"
#include "gui\guiAnimation\valueAnimation.h"

namespace Cjing3D
{
namespace Gui
{
	template<typename T>
	class ValueAnimationOption
	{
	public:
		using SelfT = ValueAnimationOption<T>;
		
		ValueAnimationOption(ValueChanelAnimation<T>& animation) : 
			mAnimation(animation),
			mSequeue(animation.GetSequence()) {}

		SelfT SetStartCallback(AnimationCallback callback)
		{
			mAnimation.SetStartCallback(callback);  
			return *this;
		}

		SelfT SetUpdateCallback(AnimationCallback callback) 
		{
			mAnimation.SetUpdateCallback(callback); 
			return *this;
		}

		SelfT SetEndCallback(AnimationCallback callback)
		{
			mAnimation.SetFinishedCallback(callback);  
			return *this;
		}

		template<template <typename, typename> class SamplerFuncT, typename EaseT>
		SelfT AddSampler(const T& value, F32 duration, EaseT easing)
		{
			mSequeue.template AddSampler<SamplerFuncT>(value, duration, easing);
			return *this;
		}

		SelfT AddSampler(const SamplerFunc<T>& sampler) { 
			mAnimation.AddSampler(sampler); 
			return *this; 
		}

	private:
		ValueChanelAnimation<T>& mAnimation;
		ValueSamplerSequence<T>& mSequeue;
	};

}
}