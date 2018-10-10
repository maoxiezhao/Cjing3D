#include "systemTime.h"

namespace Cjing3D {
	namespace {
		unsigned __int16 GetNumSystemCores()
		{
			SYSTEM_INFO systemInfo = {};
			GetSystemInfo(&systemInfo);
			return static_cast<unsigned __int16>(systemInfo.dwNumberOfProcessors);
		}

		// 内核个数
		const U16 numSystemCores = GetNumSystemCores();

		inline U64 ConvertTimestamp(const FILETIME& ftime) noexcept {
			return static_cast<U64>(ftime.dwLowDateTime)
				| static_cast<U64>(ftime.dwHighDateTime) << 32u;
		}

		// 获取进程运行时间
		const std::pair< U64, U64 > GetCoreTimestamps() noexcept {
			FILETIME ftime;
			FILETIME kernel_mode_ftime;
			FILETIME user_mode_ftime;
			/*
			lpCreationTime FILETIME，指定一个FILETIME结构，在其中装载进程的创建时间
			lpExitTime     FILETIME，指定一个FILETIME结构，在其中装载进程的中止时间
			lpKernelTime   FILETIME，指定一个FILETIME结构，在其中装载进程花在内核模式上的总时间
			lpUserTime     FILETIME，指定一个FILETIME结构，在其中装载进程花在用户模式上的总时间
			*/
			const BOOL result = GetProcessTimes(GetCurrentProcess(),
				&ftime,
				&ftime,
				&kernel_mode_ftime,
				&user_mode_ftime);

			if (result)
				return { ConvertTimestamp(kernel_mode_ftime),
				ConvertTimestamp(user_mode_ftime) };

			return {};
		}

		// 返回内核时间，内核时间等于用户模式时间+用户模式时间
		U64 GetCoreTimeStamp() {
			const auto timestamps = GetCoreTimestamps();
			return timestamps.first + timestamps.second;
		}
	}

	// block的now函数用于获取当前的时间戳
	const CoreClock::time_point CoreClock::now()
	{
		const auto time = GetCoreTimeStamp() / numSystemCores;
		return time_point(duration(time));
	}

}


