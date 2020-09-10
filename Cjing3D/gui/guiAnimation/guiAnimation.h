#pragma once

#include "gui\guiInclude.h"
#include "utils\signal\connectionList.h"

namespace Cjing3D {
namespace Gui {

	class Widget;

	using AnimationCallback = std::function<void()>;

	class AnimationBase
	{
	public:		
		enum AnimationStatus
		{
			PLAYING,
			PAUSE,
			FINISHED,
			STOPED,
		};

		AnimationBase();
		virtual ~AnimationBase();

		virtual void FixedUpdate();
		virtual void Update(F32 delta);

		virtual void Play();
		virtual void Pause();
		virtual void Stop();
		virtual void Reset();
		virtual void Finish();

		bool IsPlaying()const { return mStatus == PLAYING; }
		bool IsPaused()const { return mStatus == PAUSE; }
		bool IsStoped()const { return mStatus == STOPED; }

		virtual F32 GetDuration()const;

		bool IsFinished()const;
		void SetSpeed(F32 speed) { mSpeed = speed; }
		F32 GetSpeed()const { return mSpeed; }
		F32 GetCurrentTime()const { return mCurrentTime; }
		void SetRemovedOnFinish(bool removed) { mIsRemovedOnFinish = removed; }
		bool IsRemovedOnFinish()const { return mIsRemovedOnFinish; }
		F32 GetLocalTime()const { return std::max(0.0f, mCurrentTime - mTimeStart); }

		void SetStartCallback(AnimationCallback func) { OnStartCallback = func; }
		void SetUpdateCallback(AnimationCallback func) { OnUpdateCallback = func; }
		void SetFinishedCallback(AnimationCallback func) { OnFinishedCallback = func; }

	protected:
		virtual void UpdateImpl(F32 delta);

		AnimationCallback OnStartCallback;
		AnimationCallback OnUpdateCallback;
		AnimationCallback OnFinishedCallback;

	private:
		AnimationStatus mStatus = PLAYING;
		F32 mTimeStart = 0.0f;
		F32 mCurrentTime = 0.0f;
		F32 mPrevTime = 0.0f;
		F32 mSpeed = 1.0f;
		bool mIsRemovedOnFinish = true;
	};
	using AnimationSharedPtr = std::shared_ptr<AnimationBase>;
}
}