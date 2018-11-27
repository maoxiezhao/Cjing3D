#include "jobSystem.h"

#include <thread>
#include <mutex>
#include <string>

namespace Cjing3D
{
	JobSystem::JobSystem(SystemContext & context):
		SubSystem(context)
	{
	}

	void JobSystem::Initialize()
	{
		U32 numCores = std::thread::hardware_concurrency();
		for (int threadID = 0; threadID < numCores; threadID++)
		{
			std::thread worker(&JobSystem::ThreadInvoke, this);

#ifdef _WIN32
			// ��ʼ��win�߳�����
			HANDLE handle = (HANDLE)worker.native_handle();

			// �������̷߳��䵽����������
			DWORD_PTR mask = 1ull << threadID;
			SetThreadAffinityMask(handle, mask);

			// Ϊ�����߳�����
			const std::wstring threadName = std::to_wstring(threadID);
			SetThreadDescription(handle, threadName.c_str());
#endif

			worker.detach();
		}
	}

	void JobSystem::Uninitialize()
	{

		mThreads.clear();
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
			if (mJobBuffer.PopBack(job))
			{
				job();
			}
			else
			{
				std::unique_lock<std::mutex> lock(mWakeMutex);
				mWakeCondition.wait(lock);
			}
		}
	}

}