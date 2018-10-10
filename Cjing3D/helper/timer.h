#pragma once

#include "systemTime.h"
#define __CHRONO_TIMER__

namespace Cjing3D {

#ifdef __CHRONO_TIMER__
	using TimeStamp = typename CoreClock::time_point;
	using TimeInterval = typename CoreClock::duration;
#else
	using TimeStamp = double;
	using TimeInterval = double;
#endif
	/**
	*	\brief ����ʱ���ʾ
	*/
	struct EngineTime
	{
		TimeInterval deltaTime;
		TimeInterval totalDeltaTime;
	};

	/**
	*	\brief ����Chrono��Timer��
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
		EngineTime GetTime()const;

	private:
		void UpdateDeltaTime()const;

		CoreClock mClock;

		mutable TimeStamp mTimeStamp;
		mutable TimeInterval mDeltaTime;
		mutable TimeInterval mTotalDeltaTime;

		bool mIsRunning;
	};

}