#include "timer.h"

namespace Cjing3D {

	Timer::Timer() :
		mClock(),
		mTimeStamp(TimeStamp::min()),
		mDeltaTime(TimeInterval::zero()),
		mTotalDeltaTime(TimeInterval::zero()),
		mIsRunning(false)
	{
	}

	void Timer::Start()
	{
		if (mIsRunning) {
			return;
		}

		mDeltaTime = TimeInterval::zero();
		mTotalDeltaTime = TimeInterval::zero();
		mTimeStamp = TimeStamp::min();

		mIsRunning = true;
	}

	void Timer::Stop()
	{
		if (!mIsRunning)
			return;

		mIsRunning = false;
	}

	void Timer::Restart()
	{
		mIsRunning = false;
		Start();
	}

	TimeInterval Timer::GetDeltaTime() const
	{
		if (mIsRunning) {
			UpdateDeltaTime();
		}

		return mDeltaTime;
	}

	TimeInterval Timer::GetTotalDeltaTime() const
	{
		if (mIsRunning)
			UpdateDeltaTime();

		return mTotalDeltaTime;
	}

	EngineTime Timer::GetTime() const
	{
		if (mIsRunning) {
			UpdateDeltaTime();
		}

		return { mDeltaTime, mTotalDeltaTime };
	}

	void Timer::UpdateDeltaTime() const
	{
		const auto curTimeStamp = mClock.now();
		mDeltaTime = curTimeStamp - mTimeStamp;
		mTotalDeltaTime += mDeltaTime;
		mTimeStamp = curTimeStamp;
	}
}
















