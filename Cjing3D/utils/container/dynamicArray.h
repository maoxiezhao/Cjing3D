#pragma once

#include "core\memory.h"

namespace Cjing3D
{
namespace Container
{
	template<typename T>
	class DynamicArray
	{
	private:
		U32 mCapacity = 0;
		U32 mSize = 0;
		T* mData = nullptr;

		void Grow()
		{
			U32 newCapacity = mCapacity == 0 ? 4 : mCapacity * 2;
			mData = (T*)Memory::AlignReallocStatic(mData, newCapacity * sizeof(T), alignof(T));
			mCapacity = newCapacity;
		}

		void CallDestructors(T* begin, T* end)
		{
			for (; begin < end; ++begin) {
				begin->~T();
			}
		}

	public:
		DynamicArray() = default;
		~DynamicArray()
		{
			CallDestructors(mData, mData + mSize);
			Memory::AlignFreeStatic(mData);
		}

		DynamicArray(const DynamicArray& rhs)
		{
			*this = rhs;
		}

		DynamicArray(DynamicArray&& rhs)
		{
			swap(rhs);
		}

		DynamicArray& operator= (const DynamicArray& rhs)
		{
			if (this != &rhs)
			{
				CallDestructors(mData, mData + mSize);
				Memory::AlignFreeStatic(mData);

				mData = (T*)Memory::AlignAllocStatic(rhs.mCapacity * sizeof(T), alignof(T));
				mCapacity = rhs.mCapacity;
				mSize = rhs.mSize;

				for (U32 i = 0; i < mSize; ++i) {
					CJING_MEM_PLACEMENT_NEW<T>((char*)(mData + i), rhs.mData[i]);
				}
			}
		}

		DynamicArray& operator= (DynamicArray&& rhs)
		{
			if (this != &rhs)
			{
				CallDestructors(mData, mData + mSize);
				Memory::AlignFreeStatic(mData);

				mData = rhs.mData;
				mCapacity = rhs.mCapacity;
				mSize = rhs.mSize;

				rhs.mData = nullptr;
				rhs.mCapacity = 0;
				rhs.mSize = 0;
			}
		}

		T* begin() const { return mData; }
		T* end() const { return mData ? mData + mSize : nullptr; }

		void swap(DynamicArray<T>& rhs)
		{
			std::swap(mCapacity, rhs.mCapacity);
			std::swap(mSize, rhs.mSize);
			std::swap(mData, rhs.mData);
		}

		template <typename Comparator>
		void removeDuplicates(Comparator equals)
		{
			if (mSize == 0) return;
			for (U32 i = 0; i < mSize - 1; ++i)
			{
				for (U32 j = i + 1; j < mSize; ++j)
				{
					if (equals(mData[i], mData[j]))
					{
						swapAndPop(j);
						--j;
					}
				}
			}
		}

		void removeDuplicates()
		{
			if (mSize == 0) return;
			for (U32 i = 0; i < mSize - 1; ++i)
			{
				for (U32 j = i + 1; j < mSize; ++j)
				{
					if (mData[i] == mData[j])
					{
						swapAndPop(j);
						--j;
					}
				}
			}
		}

		void free()
		{
			clear();
			Memory::AlignFreeStatic(mData);
			mCapacity = 0;
			mData = nullptr;
		}

		template <typename F>
		int find(F predicate) const
		{
			for (U32 i = 0; i < mSize; ++i)
			{
				if (predicate(mData[i]))
				{
					return i;
				}
			}
			return -1;
		}

		template <typename R>
		int indexOf(R item) const
		{
			for (U32 i = 0; i < mSize; ++i)
			{
				if (mData[i] == item)
				{
					return i;
				}
			}
			return -1;
		}

		int indexOf(const T& item) const
		{
			for (U32 i = 0; i < mSize; ++i)
			{
				if (mData[i] == item)
				{
					return i;
				}
			}
			return -1;
		}

		template <typename F>
		void eraseItems(F predicate)
		{
			for (U32 i = mSize - 1; i != 0xffFFffFF; --i)
			{
				if (predicate(mData[i]))
				{
					erase(i);
				}
			}
		}

		void swapAndPopItem(const T& item)
		{
			for (U32 i = 0; i < mSize; ++i)
			{
				if (mData[i] == item)
				{
					swapAndPop(i);
					return;
				}
			}
		}

		void swapAndPop(U32 index)
		{
			if (index >= 0 && index < mSize)
			{
				mData[index].~T();
				if (index != mSize - 1)
				{
					Memory::Memmove(mData + index, mData + mSize - 1, sizeof(T));
				}
				--mSize;
			}
		}

		void eraseItem(const T& item)
		{
			for (U32 i = 0; i < mSize; ++i)
			{
				if (mData[i] == item)
				{
					erase(i);
					return;
				}
			}
		}

		void insert(U32 index, const T& value)
		{
			if (mSize == mCapacity) {
				Grow();
			}

			Memory::Memmove(mData + index + 1, mData + index, sizeof(T) * (mSize - index));
			CJING_MEM_PLACEMENT_NEW<T>(&mData[index], value);
			++mSize;
		}


		void erase(U32 index)
		{
			if (index < mSize)
			{
				mData[index].~T();
				if (index < mSize - 1)
				{
					Memory::Memcpy(mData + index, mData + index + 1, sizeof(T) * (mSize - index - 1));
				}
				--mSize;
			}
		}

		void push(const T& value)
		{
			U32 size = mSize;
			if (size == mCapacity) {
				Grow();
			}

			CJING_MEM_PLACEMENT_NEW<T>((char*)(mData + size), value);
			++size;
			mSize = size;
		}

		template <typename _Ty> struct remove_reference { typedef _Ty type; };
		template <typename _Ty> struct remove_reference<_Ty&> { typedef _Ty type; };
		template <typename _Ty> struct remove_reference<_Ty&&> { typedef _Ty type; };

		template <typename _Ty> 
		_Ty&& myforward(typename remove_reference<_Ty>::type& _Arg)
		{
			return (static_cast<_Ty&&>(_Arg));
		}

		template <typename... Args> 
		T& emplace(Args&&... args)
		{
			if (mSize == mCapacity) {
				Grow();
			}

			CJING_MEM_PLACEMENT_NEW<T>((char*)(mData + mSize), myforward<Args>(args)...);
			++mSize;
			return mData[mSize - 1];
		}

		template <typename... Args>
		T& emplaceAt(U32 idx, Args&&... args)
		{
			if (mSize == mCapacity) {
				Grow();
			}

			Memory::Memmove(&mData[idx + 1], &mData[idx], sizeof(mData[idx]) * (mSize - idx));

			CJING_MEM_PLACEMENT_NEW<T>((char*)(mData + idx), myforward<Args>(args)...);
			++mSize;
			return mData[idx];
		}

		bool empty() const { return mSize == 0; }

		void clear()
		{
			CallDestructors(mData, mData + mSize);
			mSize = 0;
		}

		const T& back() const { return mData[mSize - 1]; }
		T& back() { return mData[mSize - 1]; }

		void pop()
		{
			if (mSize > 0)
			{
				mData[mSize - 1].~T();
				--mSize;
			}
		}

		void resize(U32 size)
		{
			if (size > mCapacity) {
				reserve(size);
			}

			for (U32 i = mSize; i < size; ++i) {
				CJING_MEM_PLACEMENT_NEW<T>((char*)(mData + i));
			}

			CallDestructors(mData + size, mData + mSize);
			mSize = size;
		}

		void reserve(U32 capacity)
		{
			if (capacity > mCapacity)
			{
				T* newData = (T*)Memory::AlignAllocStatic(capacity * sizeof(T), alignof(T));
				Memory::Memcpy(newData, mData, sizeof(T) * mSize);
				Memory::AlignFreeStatic(mData);
				mData = newData;
				mCapacity = capacity;
			}
		}

		T* data()
		{
			return mData;
		}

		const T& operator[](U32 index) const
		{
			assert(index < mSize);
			return mData[index];
		}

		T& operator[](U32 index)
		{
			assert(index < mSize);
			return mData[index];
		}

		U32 byte_size() const { return mSize * sizeof(T); }
		int size() const { return mSize; }
		U32 capacity() const { return mCapacity; }
	};
}
}