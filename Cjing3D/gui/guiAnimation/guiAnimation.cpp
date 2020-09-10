#include "guiAnimation.h"

namespace Cjing3D
{
namespace Gui
{
	AnimationBase::AnimationBase()
	{
	}

	AnimationBase::~AnimationBase()
	{
	}

	void AnimationBase::FixedUpdate()
	{
	}

	void AnimationBase::Update(F32 delta)
	{
		if (!IsPlaying() && GetDuration() > 0) {
			return;
		}
		
		mCurrentTime += delta;

		if (mCurrentTime >= mTimeStart)
		{
			if (OnStartCallback != nullptr && mPrevTime <= 0) {
				OnStartCallback();
			}

			UpdateImpl(delta);

			if (OnUpdateCallback != nullptr) {
				OnUpdateCallback();
			}

			F32 endTime = mTimeStart + GetDuration();
			if (OnFinishedCallback != nullptr && mCurrentTime >= endTime && mPrevTime < endTime) {
				OnFinishedCallback();
			}

			mPrevTime = mCurrentTime;
		} 
	}

	void AnimationBase::Play()
	{
		mStatus = PLAYING;
	}

	void AnimationBase::Pause()
	{
		mStatus = PAUSE;
	}

	void AnimationBase::Stop()
	{
		// stop不会调用finish
		mStatus = STOPED;
		mCurrentTime = 0;
		mPrevTime = 0;
	}

	void AnimationBase::Reset()
	{
		mCurrentTime = 0;
		mPrevTime = 0;
		mStatus = PLAYING;
	}

	void AnimationBase::Finish()
	{
		if (mStatus != FINISHED && mSpeed != STOPED)
		{
			mStatus = FINISHED;
			OnFinishedCallback();
		}
	}

	F32 AnimationBase::GetDuration() const
	{
		return 0.0f;;
	}

	bool AnimationBase::IsFinished() const
	{
		if (mStatus == FINISHED || mStatus == STOPED) {
			return true;
		}
		F32 endTime = mTimeStart + GetDuration();
		return mCurrentTime >= endTime;
	}

	void AnimationBase::UpdateImpl(F32 delta)
	{
	}
}
}