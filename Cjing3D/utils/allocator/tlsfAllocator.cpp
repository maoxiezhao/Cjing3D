#include "tlsfAllocator.h"
#include "helper\debug.h"
#include "utils\tlsf\tlsf.h"
#include "core\memory.h"
#include "helper\logger.h"

namespace Cjing3D
{
	namespace 
	{
#if defined (__alpha__) || defined (__ia64__) || defined (__x86_64__) \
	|| defined (_WIN64) || defined (__LP64__) || defined (__LLP64__)
#define TLSF_64BIT
#endif
		enum tlsf_public
		{
			SL_INDEX_COUNT_LOG2 = 5,
		};

		// see tlsf.c
		enum tlsf_private
		{
#if defined (TLSF_64BIT)
			ALIGN_SIZE_LOG2 = 3,
#else
			ALIGN_SIZE_LOG2 = 2,
#endif
			ALIGN_SIZE = (1 << ALIGN_SIZE_LOG2),

#if defined (TLSF_64BIT)
			FL_INDEX_MAX = 32,
#else
			FL_INDEX_MAX = 30,
#endif
			SL_INDEX_COUNT = (1 << SL_INDEX_COUNT_LOG2),
			FL_INDEX_SHIFT = (SL_INDEX_COUNT_LOG2 + ALIGN_SIZE_LOG2),
			FL_INDEX_COUNT = (FL_INDEX_MAX - FL_INDEX_SHIFT + 1),

			SMALL_BLOCK_SIZE = (1 << FL_INDEX_SHIFT),
		};

		// 从前往后找到第一个二进制位为1的位置
		int32_t CountLeadingZeros(uint64_t mask)
		{
			unsigned long index;
			auto ret = _BitScanReverse64(&index, mask);  // 从后往前找到第一个二进制位为1的位置
			return ret ? 63 - index : 64;
		}

		size_t PotRoundUp(size_t value, size_t roundUpTo)
		{
			return (value + ((roundUpTo) - 1)) & ~(roundUpTo - 1);
		}
	}

	AllocatorTLSF::AllocatorTLSF(IAllocator& parentAllocator, size_t minPoolSize) :
		mParentAllocator(parentAllocator),
		mMinPoolSize(minPoolSize)
	{
		mTlsf = parentAllocator.AlignAllocate(minPoolSize, PLATFORM_ALIGNMENT);
		mTlsf = tlsf_create(mTlsf);
	}

	AllocatorTLSF::~AllocatorTLSF()
	{
		TlsfPool* poolPtr = mPool;
		while (poolPtr)
		{
			TlsfPool* next = poolPtr->next_;
			mParentAllocator.AlignFree(poolPtr);
			poolPtr = next;
		}
		mParentAllocator.AlignFree(mTlsf);
	}

	void* AllocatorTLSF::Allocate(size_t size)
	{
		void* mem = tlsf_malloc(mTlsf, size);
		if (mem == nullptr)
		{
			AddPool(size, PLATFORM_ALIGNMENT);
			mem = tlsf_malloc(mTlsf, size);
			ERR_FAIL_COND_V(!mem, nullptr);
		}
		return mem;
	}

	void* AllocatorTLSF::Reallocate(void* ptr, size_t newSize)
	{
		void* mem = tlsf_realloc(mTlsf, ptr, newSize);
		if (mem == nullptr)
		{
			AddPool(newSize, PLATFORM_ALIGNMENT);
			mem = tlsf_realloc(mTlsf, ptr, newSize);
			ERR_FAIL_COND_V(!mem, nullptr);
		}
		return mem;
	}

	void AllocatorTLSF::Free(void* ptr)
	{
		tlsf_free(mTlsf, ptr);
	}

	void* AllocatorTLSF::AlignAllocate(size_t size, size_t align)
	{
		void* mem = tlsf_memalign(mTlsf, size, align);
		if (mem == nullptr)
		{
			AddPool(size, PLATFORM_ALIGNMENT);
			mem = tlsf_memalign(mTlsf, size, align);
			ERR_FAIL_COND_V(!mem, nullptr);
		}
		return mem;
	}

	void* AllocatorTLSF::AlignReallocate(void* ptr, size_t newSize, size_t align)
	{
		return Reallocate(ptr, newSize);
	}

	void AllocatorTLSF::AlignFree(void* ptr)
	{
		tlsf_free(mTlsf, ptr);
	}

	void AllocatorTLSF::LogStats() const
	{
		Logger::InfoEx(" - TLSF Heap:\n");
		Logger::InfoEx(" - - Integrity: %s\n", tlsf_check(mTlsf) == 0 ? "GOOD" : "BAD");

		const auto Walker = [](void* ptr, size_t size, int used, void* user) {
			Logger::InfoEx(" - - - Alloc: %p, %lld bytes, %s\n", ptr, (int64_t)size, used ? "USED" : "FREE");
		};

		for (auto* pool = mPool; pool; pool = pool->next_)
		{
			Logger::InfoEx(" - - Pool: %p, %lld bytes\n", pool->mem_, pool->size_);
			Logger::InfoEx(" - - - Integrity: %s\n", tlsf_check_pool(pool->pool_) == 0 ? "GOOD" : "BAD");
			tlsf_walk_pool(pool->pool_, Walker, (void*)this);
		}
	}

	bool AllocatorTLSF::AddPool(size_t minPoolSize, size_t minAlign)
	{
		/* see tlsf.c: This version rounds up to the next block size (for allocations) */
		auto RoundUpBlockSize = [](int64_t size) {
			if (size > SMALL_BLOCK_SIZE)
			{
				uint64_t round = (1 << ((64 - CountLeadingZeros((uint64_t)size)) - SL_INDEX_COUNT_LOG2)) - 1;
				size += round;
			}
			return size;
		};

		size_t overhead = PotRoundUp(sizeof(TlsfPool) + tlsf_pool_overhead(), minAlign) + minAlign;
		size_t reqSize = overhead + (size_t)RoundUpBlockSize(minPoolSize);
		size_t poolSize = PotRoundUp(reqSize, mMinPoolSize);

		uint8_t* poolMem = (uint8_t*)mParentAllocator.AlignAllocate(poolSize, PLATFORM_ALIGNMENT);
		if (poolMem)
		{
			TlsfPool* newPool = CJING_MEM_PLACEMENT_NEW<TlsfPool>(poolMem);
			newPool->mem_ = (uint8_t*)(newPool + 1);
			newPool->size_ = poolSize - sizeof(TlsfPool);
			newPool->pool_ = tlsf_add_pool(mTlsf, newPool->mem_, newPool->size_);
			newPool->next_ = mPool;
			mPool = newPool;

			return true;
		}
		return false;
	}
}