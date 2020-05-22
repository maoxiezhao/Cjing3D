#include "memory.h"
#include "safeRefcount.h"
#include "utils\allocator\tlsfAllocator.h"

namespace Cjing3D
{
#ifdef CJING_DEBUG
	uint64_t memUsage = 0;
	uint64_t maxMemUsage = 0;
#endif

	DefaultAllocator allocator;

	void Memory::Initialize()
	{
	}

	void Memory::Uninitialize()
	{
#ifdef CJING_DEBUG
		if (GetMemUsage() > 0) {
			Debug::Warning("There still somethins unrelease.");
		}
#endif
	}

	void* Memory::AllocStatic(size_t size, bool prepad)
	{
#ifdef CJING_DEBUG
		prepad = true;
#endif
		void* mem = allocator.Allocate(size + (prepad ? DEFAULT_ALIGN_SIZE : 0));
		ERR_FAIL_COND_V(!mem, nullptr);

		if (prepad)
		{
#ifdef CJING_DEBUG
			atomicAdd(&memUsage, size);
			atomicExchangeIfGreater(&maxMemUsage, memUsage);
#endif
			// 在指针开始位置保存大小，返回的指针跳过开始位置
			uint64_t* s = (uint64_t*)mem;
			*s = size;
			uint8_t* s8 = (uint8_t*)mem;
			return s8 + DEFAULT_ALIGN_SIZE;
		}
		else
		{
			return mem;
		}
	}
	void* Memory::ReallocStatic(void* ptr, size_t newBytes, bool prepad)
	{
		if (ptr == nullptr) {
			return AllocStatic(newBytes, prepad);
		}

#ifdef CJING_DEBUG
		prepad = true;
#endif
		if (prepad)
		{
			// align情况下，需要后置ALIGN_SIZE返回分配内存开始的位置
			uint8_t* mem = (uint8_t*)ptr;
			mem -= DEFAULT_ALIGN_SIZE;

#ifdef CJING_DEBUG
			uint64_t oldBytes = *((uint64_t*)mem);
			if (newBytes > oldBytes)
			{
				atomicAdd(&memUsage, newBytes - oldBytes);
				atomicExchangeIfGreater(&maxMemUsage, memUsage);
			}
			else
			{
				atomicSub(&memUsage, oldBytes - newBytes);
			}
#endif
			if (newBytes == 0)
			{
				FreeStatic(ptr, prepad);
				return nullptr;
			}
			else
			{
				mem = (uint8_t*)allocator.Reallocate(mem, newBytes + (prepad ? DEFAULT_ALIGN_SIZE : 0));
				ERR_FAIL_COND_V(!mem, nullptr);

				*((uint64_t*)mem) = newBytes;
				return mem + DEFAULT_ALIGN_SIZE;
			}
		}
		else
		{
			void* mem = allocator.Reallocate(ptr, newBytes);
			ERR_FAIL_COND_V(!mem, nullptr);

			return mem;
		}
	}

	void Memory::FreeStatic(void* ptr, bool prepad)
	{
		if (ptr == nullptr) {
			return;
		}

#ifdef CJING_DEBUG
		prepad = true;
#endif

		uint8_t* mem = (uint8_t*)ptr;
		if (prepad)
		{
			mem -= DEFAULT_ALIGN_SIZE;

#ifdef CJING_DEBUG
			uint64_t oldBytes = *((uint64_t*)mem);
			atomicSub(&memUsage, oldBytes);
#endif
			allocator.Free(mem);
		}
		else
		{
			allocator.Free(mem);
		}
	}

	void* Memory::AlignAllocStatic(size_t size, size_t align, bool prepad)
	{
#ifdef CJING_DEBUG
		prepad = true;
#endif
		void* mem = allocator.AlignAllocate(size + (prepad ? DEFAULT_ALIGN_SIZE : 0), align);
		ERR_FAIL_COND_V(!mem, nullptr);

		if (prepad)
		{
#ifdef CJING_DEBUG
			atomicAdd(&memUsage, size);
			atomicExchangeIfGreater(&maxMemUsage, memUsage);
#endif
			// 在指针开始位置保存大小，返回的指针跳过开始位置
			uint64_t* s = (uint64_t*)mem;
			*s = size;
			uint8_t* s8 = (uint8_t*)mem;
			return s8 + DEFAULT_ALIGN_SIZE;
		}
		else
		{
			return mem;
		}
	}

	void* Memory::AlignReallocStatic(void* ptr, size_t newBytes, size_t align, bool prepad)
	{
		if (ptr == nullptr) {
			return AllocStatic(newBytes, prepad);
		}

#ifdef CJING_DEBUG
		prepad = true;
#endif
		if (prepad)
		{
			// align情况下，需要后置ALIGN_SIZE返回分配内存开始的位置
			uint8_t* mem = (uint8_t*)ptr;
			mem -= DEFAULT_ALIGN_SIZE;

#ifdef CJING_DEBUG
			uint64_t oldBytes = *((uint64_t*)mem);
			if (newBytes > oldBytes)
			{
				atomicAdd(&memUsage, newBytes - oldBytes);
				atomicExchangeIfGreater(&maxMemUsage, memUsage);
			}
			else
			{
				atomicSub(&memUsage, oldBytes - newBytes);
			}
#endif
			if (newBytes == 0)
			{
				FreeStatic(ptr, prepad);
				return nullptr;
			}
			else
			{
				mem = (uint8_t*)allocator.AlignReallocate(mem, newBytes + (prepad ? DEFAULT_ALIGN_SIZE : 0), align);
				ERR_FAIL_COND_V(!mem, nullptr);

				*((uint64_t*)mem) = newBytes;
				return mem + DEFAULT_ALIGN_SIZE;
			}
		}
		else
		{
			void* mem = allocator.AlignReallocate(ptr, newBytes, align);
			ERR_FAIL_COND_V(!mem, nullptr);

			return mem;
		}
	}

	void Memory::AlignFreeStatic(void* ptr, bool prepad)
	{
		if (ptr == nullptr) {
			return;
		}

#ifdef CJING_DEBUG
		prepad = true;
#endif

		uint8_t* mem = (uint8_t*)ptr;
		if (prepad)
		{
			mem -= DEFAULT_ALIGN_SIZE;

#ifdef CJING_DEBUG
			uint64_t oldBytes = *((uint64_t*)mem);
			atomicSub(&memUsage, oldBytes);
#endif
			allocator.AlignFree(mem);
		}
		else
		{
			allocator.AlignFree(mem);
		}
	}

	void Memory::Memmove(void* dst, void* src, size_t size)
	{
		memmove_s(dst, size, src, size);
	}

	void Memory::Memcpy(void* dst, void* src, size_t size)
	{
		memcpy_s(dst, size, src, size);
	}

	uint64_t Memory::GetMemUsage()
	{
#ifdef CJING_DEBUG
		return memUsage;
#else
		return 0;
#endif
	}

	uint64_t Memory::GetMaxMemUsage()
	{
#ifdef CJING_DEBUG
		return maxMemUsage;
#else
		return 0;
#endif
	}
}

//void* operator new(size_t p_size)
//{
//	Cjing3D::Logger::Info("There is allocation with base new.");
//	return Cjing3D::Memory::AllocStatic(p_size);
//}

void* operator new(size_t p_size, const char* p_description)
{
	return Cjing3D::Memory::AllocStatic(p_size);
}

void* operator new(size_t p_size, void* p_pointer, size_t check, const char* p_description)
{
	return p_pointer;
}

void operator delete(void* p_mem, const char* p_description)
{
	Cjing3D::Debug::Error("Should not to cal placement delete.");
}
