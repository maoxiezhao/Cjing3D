#pragma once

#include "common\common.h"
#include "utils\thread\spinLock.h"


namespace Cjing3D
{

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