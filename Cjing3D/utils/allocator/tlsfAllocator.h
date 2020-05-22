#pragma once

#include "core\allocator.h"

namespace Cjing3D
{
	/**
	 * two-level segregated fit by Matthew Conte (http://tlsf.baisoku.org).
	 * It is not thread safe.
	 */

	extern "C" {
		/* tlsf_t: a TLSF structure. Can contain 1 to N pools. */
		/* pool_t: a block of memory that TLSF can manage. */
		typedef void* tlsf_t;
		typedef void* pool_t;
	}

	class AllocatorTLSF : public IAllocator
	{
	public:
		AllocatorTLSF(IAllocator& parentAllocator, size_t minPoolSize);
		virtual ~AllocatorTLSF();

		virtual void* Allocate(size_t size);
		virtual void* Reallocate(void* ptr, size_t newSize);
		virtual void Free(void* ptr);

		virtual void* AlignAllocate(size_t size, size_t align);
		virtual void* AlignReallocate(void* ptr, size_t newSize, size_t align);
		virtual void  AlignFree(void* ptr);

		virtual void LogStats() const;

	private:
		const size_t mMinPoolSize;
		IAllocator& mParentAllocator;

		bool AddPool(size_t minPoolSize, size_t minAlign = PLATFORM_ALIGNMENT);

		struct TlsfPool
		{
			uint8_t* mem_ = nullptr;
			size_t size_ = 0;
			pool_t pool_ = nullptr;
			TlsfPool* next_ = nullptr;
		};

		tlsf_t mTlsf = nullptr;
		TlsfPool* mPool = nullptr;
	};

}