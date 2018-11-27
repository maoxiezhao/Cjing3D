#pragma once

#include "core\subSystem.hpp"
#include "utils\threadSafeBuffer.h"

#include <functional>
#include <condition_variable>
#include <mutex>

namespace Cjing3D
{

using TaskJob = std::function<void()>;

class JobSystem : public SubSystem
{
public:
	JobSystem(SystemContext& context);

	virtual void Initialize();
	virtual void Uninitialize();

	void Execute(const TaskJob& task);

	bool IsBusy()const;
	void Wait();

	U32 GetThreadCount();
	static const int MaxJobCount = 128;

	// the function invoked by threads
	void ThreadInvoke();

private:
	ThreadSafeBuffer<TaskJob, MaxJobCount> mJobBuffer;
	std::condition_variable mWakeCondition;
	std::mutex mWakeMutex;

	std::vector<std::thread> mThreads;
};

}