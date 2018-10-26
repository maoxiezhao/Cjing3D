#pragma once

#include "common\common.h"

#include <mutex>
#include <atomic>

namespace Cjing3D
{

class SpinLock
{
public:
	SpinLock();
	~SpinLock();

	void Lock();
	void UnLock();

private:
	std::atomic_flag mFlag;
};

// 线程安全系统，所有系统从该类派生
class ThreadSafeSystem
{
public:
	ThreadSafeSystem();
	~ThreadSafeSystem();

	void Lock();
	void UnLock();

private:
	SpinLock mSpinLock;
};

}