#include "threadSafeSystem.h"

namespace Cjing3D
{

ThreadSafeSystem::ThreadSafeSystem()
{
}

ThreadSafeSystem::~ThreadSafeSystem() = default;

void ThreadSafeSystem::Lock()
{
	mSpinLock.lock();
}

void ThreadSafeSystem::UnLock()
{
	mSpinLock.unlock();
}
}