#pragma once

#include "core\memory.h"

namespace Cjing3D
{
namespace Container
{
	/**
	 * Fixed size array.
	 */
	template<typename T, I32 SIZE>
	class StaticArray
	{
	public:
		using IndexT = decltype(SIZE);
		using ValueT = T;
		using Iterator = ValueT*;
		using ConstIterator = const ValueT*;

		StaticArray() = default;
		StaticArray(const StaticArray<T, SIZE>& rhs)
		{
			Memory::Memcpy(mData, rhs.mData, sizeof(T) * SIZE);
		}
		StaticArray(StaticArray<T, SIZE>&& rhs)
		{
			swap(rhs);
		}

		StaticArray<T, SIZE>& operator= (const StaticArray<T, SIZE>& rhs)
		{
			Memory::Memcpy(mData, rhs.mData, sizeof(T) * SIZE);
		}
		StaticArray<T, SIZE>& operator= (StaticArray<T, SIZE>&& rhs)
		{
			swap(rhs);
		}

		void swap(StaticArray<T,SIZE>& rhs)
		{
			std::swap(mData, rhs.mData);
		}

		T& operator[](IndexT Idx)
		{
			Debug::ThrowIfFailed(Idx >= 0 && Idx < SIZE, "Index out of bounds. (index %u, size %u)", Idx, SIZE);
			return mData[Idx];
		}

		const T& operator[](IndexT Idx) const
		{
			Debug::ThrowIfFailed(Idx >= 0 && Idx < SIZE, "Index out of bounds. (%u, size %u)", Idx, SIZE);
			return mData[Idx];
		}

		void fill(const T& Val)
		{
			for (IndexT Idx = 0; Idx < SIZE; ++Idx)
				mData[Idx] = Val;
		}

		T& front() { return mData[0]; }
		const T& front() const { return mData[0]; }
		T& back() { return mData[SIZE - 1]; }
		const T& back() const { return mData[SIZE - 1]; }

		Iterator begin() { return mData; }
		ConstIterator begin() const { return mData; }
		Iterator end() { return mData + SIZE; }
		ConstIterator end() const { return mData + SIZE; }

		T* data() { return &mData[0]; }
		const T* data() const { return &mData[0]; }
		IndexT size() const { return SIZE; }

		T mData[SIZE];
	};
}
}