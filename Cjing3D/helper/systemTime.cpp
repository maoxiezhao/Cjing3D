#include "systemTime.h"

namespace Cjing3D {
	namespace {
		unsigned __int16 GetNumSystemCores()
		{
			SYSTEM_INFO systemInfo = {};
			GetSystemInfo(&systemInfo);
			return static_cast<unsigned __int16>(systemInfo.dwNumberOfProcessors);
		}

		// �ں˸���
		const U16 numSystemCores = GetNumSystemCores();

		inline U64 ConvertTimestamp(const FILETIME& ftime) noexcept {
			return static_cast<U64>(ftime.dwLowDateTime)
				| static_cast<U64>(ftime.dwHighDateTime) << 32u;
		}

		// ��ȡ��������ʱ��
		const std::pair< U64, U64 > GetCoreTimestamps() noexcept {
			FILETIME ftime;
			FILETIME kernel_mode_ftime;
			FILETIME user_mode_ftime;
			/*
			lpCreationTime FILETIME��ָ��һ��FILETIME�ṹ��������װ�ؽ��̵Ĵ���ʱ��
			lpExitTime     FILETIME��ָ��һ��FILETIME�ṹ��������װ�ؽ��̵���ֹʱ��
			lpKernelTime   FILETIME��ָ��һ��FILETIME�ṹ��������װ�ؽ��̻����ں�ģʽ�ϵ���ʱ��
			lpUserTime     FILETIME��ָ��һ��FILETIME�ṹ��������װ�ؽ��̻����û�ģʽ�ϵ���ʱ��
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

		// �����ں�ʱ�䣬�ں�ʱ������û�ģʽʱ��+�û�ģʽʱ��
		U64 GetCoreTimeStamp() {
			const auto timestamps = GetCoreTimestamps();
			return timestamps.first + timestamps.second;
		}
	}

	// block��now�������ڻ�ȡ��ǰ��ʱ���
	const CoreClock::time_point CoreClock::now()
	{
		const auto time = GetCoreTimeStamp() / numSystemCores;
		return time_point(duration(time));
	}

}


