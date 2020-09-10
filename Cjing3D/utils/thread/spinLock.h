#pragma once

#include <mutex>
#include <atomic>

namespace Cjing3D
{
	class SpinLock
	{
	public:
		SpinLock() = default;
		~SpinLock() { unlock(); }

		void lock()
		{
			while (!TryLock()) {
				std::this_thread::yield();
			}
		}

		bool TryLock()
		{
			return !mFlag.test_and_set(std::memory_order_acquire);
		}

		void unlock()
		{
			mFlag.clear(std::memory_order_release);
		}

	private:
		std::atomic_flag mFlag = ATOMIC_FLAG_INIT;
	};

	using GuardSpinLock = std::lock_guard<SpinLock>;
}