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

	JobSystem::JobSystem(SystemContext & context):
		SubSystem(context),
		mThreadCount(0)
	{
	}

	void JobSystem::Initialize()
	{
		finishedLabel.store(0);

		U32 numCores = std::thread::hardware_concurrency();
		for (int threadID = 0; threadID < numCores; threadID++)
		{
			std::thread worker(&JobSystem::ThreadInvoke, this);

#ifdef _WIN32
			// ³õÊ¼»¯winÏß³ÌÉèÖÃ
			HANDLE handle = (HANDLE)worker.native_handle();

			// ½«¸÷¸öÏß³Ì·ÖÅäµ½¸÷¸öºËÐÄÖÐ
			DWORD_PTR mask = 1ull << threadID;
			SetThreadAffinityMask(handle, mask);

			// Îª¸÷¸öÏß³ÌÃüÃû
			const std::wstring threadName = std::to_wstring(threadID);
			SetThreadDescription(handle, threadName.c_str());
#endif

			worker.detach();
		}
		mThreadCount = numCores;
	}

	void JobSystem::Uninitialize()
	{
		threadStoppingFlag = true;
	}

	U32 JobSystem::GetThreadCount()
	{
		return mThreads.size();
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

	void Execute(const TaskJob& task)
	{
		currentLabel++;
		while(mJobPool.PushBack(task) == false)
		{
			Poll();
		}	

		mWakeCondition.notify_one();	
	}

	// 主线程时，不断唤醒其他线程去执行job
	void Poll()
	{
		mWakeCondition.notify_one();
		std::this_thread::yield();	
	}
}