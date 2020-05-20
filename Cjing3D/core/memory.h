#pragma once

#include "core\allocator.h"
#include "helper\debug.h"

namespace Cjing3D
{

#ifndef DEFAULT_ALIGN_SIZE
#define DEFAULT_ALIGN_SIZE 16
#endif

	class Memory
	{
	public:
		static void Initialize();
		static void Uninitialize();

		static void* AllocStatic(size_t size, bool prepad = false);
		static void* ReallocStatic(void* ptr, size_t newBytes, bool prepad = false);
		static void FreeStatic(void* ptr, bool prepad = false);

		static void* AlignAllocStatic(size_t size, size_t align, bool prepad = false);
		static void AlignFreeStatic(void* ptr, bool prepad = false);

		static uint64_t GetMemUsage();
		static uint64_t GetMaxMemUsage();
	};

#define CJING_MEM_ALLOC(size) Memory::AllocStatic(size)
#define CJING_MEM_REALLOC(ptr, size) Memory::ReallocStatic(ptr, size)
#define CJING_MEM_FREE(ptr) Memory::FreeStatic(ptr)
#define CJING_MEM_NEW(TargetClass) new("") TargetClass

	template<typename T, typename... Args>
	T* CJING_MEM_PLACEMENT_NEW(void* mem, Args&&... args)
	{
		return new(mem) T{ std::forward<Args>(args)... };
	}

	template<typename T>
	void CJING_MEM_DELETE(T* ptr)
	{
		if (ptr == nullptr) {
			return;
		}
		if (!__has_trivial_destructor(T)) {
			ptr->~T();
		}

		return Memory::FreeStatic(ptr);
	}

	template<typename T>
	T* CJING_MEM_NEW_ARR(size_t num, const char* desc = "")
	{
		size_t size = sizeof(T) * num;
		uint64_t* mem = (uint64_t)Memory::AllocStatic(size, true);
		ERR_FAIL_COND_V(!mem, nullptr);

		*(mem - 1) == num;

		T* elems = (T*)mem;
		if (!__has_trivial_constructor(T)) {
			for (size_t i = 0; i < num; i++) {
				new(&elems[i], sizeof(T), desc) T;
			}
		}

		return elems;
	}

	template<typename T>
	void CJING_MEM_DELETE_ARR(T* ptrArr)
	{
		uint64_t* mem = (uint64_t*)ptrArr;
		if (mem == nullptr) {
			return;
		}

		if (!__has_trivial_destructor(T)) {
			size_t num = *(mem - 1);
		
			for (size_t i = 0; i < num; i++) {
				ptrArr[i].~T();
			}
		}

		Memory::FreeStatic(mem, true);
	}

	template<typename T>
	void CJING_MEM_ARR_LEN(const T* ptrArr)
	{
		if (ptrArr == nullptr) {
			return 0;
		}

		uint64_t* mem = (uint64_t*)ptrArr;
		return *(mem - 1);
	}

	template<typename T>
	using SharedPtr = std::shared_ptr<T>;

	template< typename T, typename... Args >
	SharedPtr<T> CJING_MAKE_SHARED(Args&&... args)
	{
		return SharedPtr<T>(CJING_MEM_NEW(T{ std::forward<Args>(args)... }), [](T* data){
			CJING_MEM_DELETE(data);
		});
	}

	template<typename T>
	using UniquePtr = std::unique_ptr<T>;

	template< typename T, typename... Args >
	UniquePtr<T> CJING_MAKE_UNIQUE(Args&&... args)
	{
		return UniquePtr<T>(CJING_MEM_NEW(T{ std::forward<Args>(args)... }), [](T* data) {
			CJING_MEM_DELETE(data);
		});
	}
}

// overloading operator new and delete
//void* operator new(size_t p_size);
void* operator new(size_t p_size, const char* p_description);
void* operator new(size_t p_size, void* p_pointer, size_t check, const char* p_description);

#ifdef _MSC_VER
void operator delete(void* p_mem, const char* p_description);
#endif
