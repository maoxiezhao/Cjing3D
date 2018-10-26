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

// �̰߳�ȫϵͳ������ϵͳ�Ӹ�������
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