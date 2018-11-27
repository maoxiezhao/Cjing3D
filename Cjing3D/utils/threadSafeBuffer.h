#pragma once

#include <stdint.h>
#include <mutex>

namespace Cjing3D {

template<typename T, size_t capacity>
class ThreadSafeBuffer
{
public:
	bool PushBack(const T& item) 
	{
		bool result = false;
		mLock.lock();
		
		size_t next = (mHead + 1) % capacity;
		if (next != mHead)
		{
			mBuffers[mHead] = item;
			mHead = next;
			result = true;
		}

		mLock.unlock();
		return result;
	}

	bool PopBack(T& item)
	{
		bool result = false;
		mLock.lock();

		if (mTail != mHead)
		{
			item = mBuffers[mTail];
			mTail = (mTail + 1) % capacity;
			result = true;
		}

		mLock.unlock();
		return result;
	}

private:
	T mBuffers[capacity];
	std::mutex mLock;
	size_t mHead = 0;
	size_t mTail = 0;
};

}