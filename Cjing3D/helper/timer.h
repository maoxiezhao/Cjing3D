#pragma once

#include "systemTime.h"
//#define __CHRONO_TIMER__

namespace Cjing3D {

#ifdef __CHRONO_TIMER__
	using TimeStamp = typename CoreClock::time_point;
	using TimeInterval = typename CoreClock::duration;
#else
	using TimeStamp = double;
	using TimeInterval = double;
#endif
	/**
	*	\brief 引擎时间表示
	*/
	struct EngineTime
	{
		TimeInterval deltaTime;
		TimeInterval totalDeltaTime;
	};

	/**
	*	\brief 基于Chrono的Timer类
	*/
	class Timer
	{
	public:
		Timer();
		~Timer() = default;

		void Start();
		void Stop();
		void Restart();

		TimeInterval GetDeltaTime()const;
		TimeInterval GetTotalDeltaTime()const;
		TimeStamp GetRecordedTimeStamp()const;
		EngineTime GetTime()const;

		void RecordDeltaTime()const;

	private:
		TimeStamp GetTotalTime()const;

		CoreClock mClock;
		mutable TimeStamp mTimeStamp;
		mutable TimeInterval mDeltaTime;
		mutable TimeInterval mTotalDeltaTime;

		static double mFrequency;
		static U64 mCounterStart;

		bool mIsRunning;
	};

}