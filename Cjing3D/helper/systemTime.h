#pragma once

#include "common\common.h"
#include <chrono>

namespace Cjing3D {

	// 定义一个CoreClock用于TimePoint
	struct CoreClock
	{
		using rep = U64;
		using period = std::ratio<1, 10'000'000>;
		using duration = std::chrono::duration<rep, period>;
		using time_point = std::chrono::time_point<CoreClock>;

		// true if t1 <= t2 is always true and the 
		// time between clock ticks is constant
		static const bool is_steady = true;	

		static const time_point now();
	};
}