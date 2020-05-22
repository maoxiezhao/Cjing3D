#include "allocator.h"

namespace Cjing3D
{
	void* DefaultAllocator::Allocate(size_t size)
	{
		return malloc(size);
	}

	void* DefaultAllocator::Reallocate(void* ptr, size_t newSize)
	{
		return realloc(ptr, newSize);
	}

	void DefaultAllocator::Free(void* ptr)
	{
		return free(ptr);
	}

	void* DefaultAllocator::AlignAllocate(size_t size, size_t align)
	{
		return _aligned_malloc(size, align);
	}

	void* DefaultAllocator::AlignReallocate(void* ptr, size_t newSize, size_t align)
	{
		return _aligned_realloc(ptr, newSize, align);
	}

	void DefaultAllocator::AlignFree(void* ptr)
	{
		return _aligned_free(ptr);
	}

	void DefaultAllocator::LogStats() const
	{
	}

	DefaultAllocator& GetDefaultAllocator()
	{
		static DefaultAllocator allocator;
		return allocator;
	}
}
