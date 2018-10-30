#include "threadSafeSystem.h"

namespace Cjing3D
{

SpinLock::SpinLock()
{
}

SpinLock::~SpinLock() 
{
	UnLock();
}

void SpinLock::Lock()
{
	while (!TryLock()) {}
}

bool SpinLock::TryLock()
{
	return !mFlag.test_and_set(std::memory_order_acquire);
}

void SpinLock::UnLock()
{
	mFlag.clear(std::memory_order_release);
}

ThreadSafeSystem::ThreadSafeSystem()
{
}

ThreadSafeSystem::~ThreadSafeSystem() = default;

void ThreadSafeSystem::Lock()
{
	mSpinLock.Lock();
}

void ThreadSafeSystem::UnLock()
{
	mSpinLock.UnLock();
}
}