#include "timer.h"
#include "helper\debug.h"

namespace Cjing3D {

	double Timer::mFrequency = 0;
	U64 Timer::mCounterStart = 0;


#ifdef __CHRONO_TIMER__
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

	void Timer::UpdateDeltaTime() const
	{
		const auto curTimeStamp = mClock.now();
		mDeltaTime = curTimeStamp - mTimeStamp;
		mTotalDeltaTime += mDeltaTime;
		mTimeStamp = curTimeStamp;
	}

#else
	Timer::Timer() :
		mTimeStamp(0),
		mDeltaTime(0),
		mTotalDeltaTime(0),
		mIsRunning(false)
	{
	}

	void Timer::Start()
	{
		if (mIsRunning) {
			return;
		}

		LARGE_INTEGER li;
		if (!QueryPerformanceFrequency(&li)) {
			Debug::Error("QueryPerformanceFrequency failed!");
		}
		mFrequency = double(li.QuadPart) / 1000.0f;

		QueryPerformanceCounter(&li);
		mCounterStart = li.QuadPart;

		mIsRunning = true;
	}

	void Timer::UpdateDeltaTime() const
	{
		TimeStamp curTimeStamp = GetTotalTime();

		mDeltaTime = curTimeStamp - mTimeStamp;
		mTotalDeltaTime += mDeltaTime;
		mTimeStamp = curTimeStamp;
	}
#endif

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

	TimeStamp Timer::GetTotalTime() const
	{
		LARGE_INTEGER li;
		QueryPerformanceCounter(&li);
		return TimeStamp(li.QuadPart - mCounterStart) / mFrequency;
	}
}
















