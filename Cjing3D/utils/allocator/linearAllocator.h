#pragma once

#include "common\common.h"

#include <cstdint>

namespace Cjing3D
{

	/**
	*	\brief 简易的线性内存分配器
	*/
	class LinearAllocator
	{
	public:
		LinearAllocator() = default;
		~LinearAllocator()
		{
			FreeBuffer();
		}

		inline void FreeBuffer()
		{
			if (mBuffer != nullptr)
			{
				CJING_MEM_DELETE_ARR(mBuffer);
				mBuffer = nullptr;
				mCapacity = 0;
				mOffset = 0;
			}
		}

		inline void Reserve(size_t capacity)
		{
			if (mCapacity > 0 && mBuffer != nullptr)
			{
				CJING_MEM_DELETE_ARR(mBuffer);
				mCapacity = 0;
				mOffset = 0;
			}

			mCapacity = capacity;
			mBuffer = CJING_MEM_NEW_ARR<uint8_t>(capacity);
		}

		inline uint8_t* Allocate(size_t size)
		{
			if (mOffset + size <= mCapacity)
			{
				uint8_t* data = &mBuffer[mOffset];
				mOffset += size;
				return data;
			}
			return nullptr;
		}

		inline void Free(size_t size)
		{
			size = std::min(mOffset, size);
			if (size > 0) {
				mOffset -= size;
			}
		}

		inline void Reset()
		{
			mOffset = 0;
		}

		inline size_t GetCapacity() const 
		{
			return mCapacity;
		}

	private:
		uint8_t* mBuffer = nullptr;
		size_t mCapacity = 0;
		size_t mOffset = 0;
	};
}