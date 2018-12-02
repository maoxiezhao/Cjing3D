#include "jobSystem.h"

#include <thread>
#include <mutex>
#include <string>

namespace Cjing3D
{
	namespace {
		bool threadStoppingFlag = false;
		uint64_t currentLabel = 0;
		std::atomic<uint64_t> finishedLabel;
	}

	JobSystem::JobSystem(SystemContext & context, bool multThread):
		SubSystem(context),
		mThreadCount(0),
		mIsMultThread(multThread)
	{
	}

	void JobSystem::Initialize()
	{
		finishedLabel.store(0);

		if (mIsMultThread)
		{
			U32 numCores = std::thread::hardware_concurrency();
			for (int threadID = 0; threadID < numCores; threadID++)
			{
				std::thread worker(&JobSystem::ThreadInvoke, this);

#ifdef _WIN32
				HANDLE handle = (HANDLE)worker.native_handle();

				// 分配各个线程到不同的处理核心
				DWORD_PTR mask = 1ull << threadID;
				SetThreadAffinityMask(handle, mask);

				const std::wstring threadName = std::to_wstring(threadID);
				SetThreadDescription(handle, threadName.c_str());
#endif

				worker.detach();
			}
			mThreadCount = numCores;
		}
		else
		{
			mThreadCount = 1;
		}
	}

	void JobSystem::Uninitialize()
	{
		threadStoppingFlag = true;
	}

	U32 JobSystem::GetThreadCount()
	{
		return mThreadCount;
	}

	void JobSystem::ThreadInvoke()
	{
		TaskJob job;
		while (true)
		{
			if (mJobPool.PopBack(job) == true) {
				job();
				finishedLabel.fetch_add(1);
			}
			else
			{
				if (threadStoppingFlag == true){
					return;
				}

				std::unique_lock<std::mutex> lock(mWakeMutex);
				mWakeCondition.wait(lock);
			}
		}
	}

	bool JobSystem::IsBusy()const
	{
		return finishedLabel.load() < currentLabel;
	}

	void JobSystem::Wait()
	{
		while(IsBusy()){
			Poll();
		}
	}

	void JobSystem::Execute(const TaskJob& task)
	{
		if (mIsMultThread)
		{
			currentLabel++;
			while (mJobPool.PushBack(task) == false)
			{
				Poll();
			}

			mWakeCondition.notify_one();
		}
		else
		{
			task();
		}
	}

	// 主线程时，不断唤醒其他线程去执行job
	void JobSystem::Poll()
	{
		mWakeCondition.notify_one();
		std::this_thread::yield();	
	}
}