#pragma once

#include "gui\guiAnimation\guiAnimation.h"
#include "gui\guiAnimation\valueAnimation.h"
#include "gui\guiAnimation\guiAnimaionOptions.h"

namespace Cjing3D
{
namespace Gui
{
	class TimeLine
	{
	public:
		TimeLine();
		~TimeLine();

		void Initialize();
		void Uninitialize();
		void FixedUpdate();
		void Update(F32 delta);
		void PlayAll();
		void PauseAll();
		void StopAll();

		// add animation
		void AddAnimation(AnimationSharedPtr animation);
		void RemoveAnimation(AnimationSharedPtr animation);
		
		// valueAnimation
		template<typename T>
		ValueAnimationOption<T> CreateAnimation(SamplerOutputValue<T>* output);

	private:
		std::vector<AnimationSharedPtr> mAnimations;
		std::vector<AnimationSharedPtr> mSubscribeRequests;

		bool mIsPausing = false;
	};

	using TimeLinePtr = std::shared_ptr<TimeLine>;
	template<typename T>
	inline ValueAnimationOption<T> TimeLine::CreateAnimation(SamplerOutputValue<T>* output)
	{
		auto animation = std::make_shared<ValueChanelAnimation<T>>(output);
		AddAnimation(animation);

		return ValueAnimationOption<T>(*animation);
	}
}
}