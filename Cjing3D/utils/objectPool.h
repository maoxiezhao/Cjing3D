#pragma once

#include "core\memory.h"

#include <type_traits>

namespace Cjing3D
{
	namespace ObjectPoolImpl
	{
		using IndexT = uint32_t;
		const uint32_t MIN_BLOCK_ALIGN = 64;

		template<typename T>
		class ObjectPoolBlock;

		template<typename T>
		std::enable_if_t<std::is_trivially_destructible<T>::value, void>
			DestructAll(ObjectPoolBlock<T>& t) {}

		template<typename T>
		std::enable_if_t<!std::is_trivially_destructible<T>::value, void>
			DestructAll(ObjectPoolBlock<T>& t) 
		{
			t.ForEach([](T* ptr) {
				ptr->~T();
			});
		}

		// 每个PoolBlock分别保存Indices和数组T*
		// Indices以索引方式构成一个可用索引列表，每一个槽位总是指向下一个可用的列表
		// 如果已经分配，Indices[index] == index
		template<typename T>
		class ObjectPoolBlock
		{
		private:
			IndexT* IndiceOffset()const
			{
				auto mem = const_cast<ObjectPoolBlock<T>*>(this + 1);
				return reinterpret_cast<IndexT*>(mem);
			}

			T* MemoryOffset() const
			{
				// skip indice(PerBlockSize)
				return reinterpret_cast<T*>(IndiceOffset() + PerBlockSize);
			}

			const IndexT PerBlockSize;
			IndexT mFreeIndex = 0;		// 当前可用的T*的索引

		public:
			ObjectPoolBlock(size_t blockSize) :
				PerBlockSize(blockSize),
				mFreeIndex(0)
			{
				// 初始化时，每一个Indice槽总是指向下一个槽位
				IndexT* indices = IndiceOffset();
				for (IndexT index = 0; index < PerBlockSize; index++) {
					indices[index] = index + 1;
				}
			}

			~ObjectPoolBlock()
			{
				DestructAll(*this);
			}

			static ObjectPoolBlock<T>* Create(size_t blockSize)
			{
				const size_t objAlign = alignof(ObjectPoolBlock<T>);
				// class mem + indice mem + objs mem
				const size_t classSize = sizeof(ObjectPoolBlock<T>);
				const size_t indiceSize = ALIGN_TO(sizeof(IndexT) * blockSize, objAlign);
				const size_t objMemSize = sizeof(T) * blockSize;
				 
				const size_t totalSize = classSize + indiceSize + objMemSize;
				ObjectPoolBlock<T>* ptr = reinterpret_cast<ObjectPoolBlock<T>*>(
					Memory::AlignAllocStatic(totalSize, MIN_BLOCK_ALIGN));
				if (ptr != nullptr)
				{
					CJING_MEM_PLACEMENT_NEW<ObjectPoolBlock>(ptr, blockSize);

					// check is correct
					Debug::CheckAssertion((uint8_t*)ptr->IndiceOffset() == (uint8_t*)ptr + classSize);
					Debug::CheckAssertion((uint8_t*)ptr->MemoryOffset() == (uint8_t*)ptr + classSize + indiceSize);
				}
				return ptr;
			}

			static void Destroy(ObjectPoolBlock<T>* ptr)
			{
				if (ptr == nullptr) {
					return;
				}

				ptr->~ObjectPoolBlock();
				Memory::AlignFreeStatic(ptr);
			}

			template<typename... Args>
			T* New(Args... args)
			{
				IndexT freeIndex = mFreeIndex;
				if (freeIndex != PerBlockSize)
				{
					IndexT* indices = IndiceOffset();
					Debug::CheckAssertion(indices[freeIndex] != freeIndex);
					mFreeIndex = indices[freeIndex];
					indices[freeIndex] = freeIndex;	// allocated: indices[freeIndex] == freeIndex

					T* mem = MemoryOffset() + freeIndex;
					return CJING_MEM_PLACEMENT_NEW<T>(mem, std::forward<Args>(args)...);
				}
				return nullptr;
			}

			void Delete(const T* obj)
			{
				if (obj == nullptr) {
					return;
				}

				const T* memBegin = MemoryOffset();
				Debug::CheckAssertion(obj >= memBegin && obj < (memBegin + PerBlockSize));

				if (!__has_trivial_destructor(T)) {
					obj->~T();
				}

				IndexT currentIndex = static_cast<IndexT>(obj - memBegin);
				IndexT* indices = IndiceOffset();
				Debug::CheckAssertion(indices[currentIndex] == currentIndex);

				// 记录下一个可用的freeIndex
				indices[currentIndex] = mFreeIndex;
				mFreeIndex = currentIndex;
			}

			void DeleteAll()
			{
				DestructAll(*this);

				mFreeIndex = 0;
				IndexT* indices = IndiceOffset();
				for (IndexT index = 0; index < PerBlockSize; index++) {
					indices[index] = index + 1;
				}
			}

			template<typename F>
			void ForEach(const F func)
			{
				IndexT* indices = IndiceOffset();
				T* mem = MemoryOffset();
				for (IndexT index = 0; index < PerBlockSize; index++)
				{
					if (indices[index] == index) {
						func(mem + index);
					}
				}
			}

			size_t GetAllocatedNum()
			{
				size_t num = 0;
				IndexT* indices = IndiceOffset();
				for (IndexT index = 0; index < PerBlockSize; index++) 
				{
					if (indices[index] == index) {
						num++;
					}
				}
				return num;
			}

			const T* GetMemOffset()
			{
				return MemoryOffset();
			}
		};

	}

	struct ObjectPoolStats
	{
		size_t numBlocks = 0;
		size_t numAllocations = 0;
	};


	// 简易的对象池。一个对象池会管理一系列block，每个block会分配一定量的空间
	template<typename T>
	class ObjectPool
	{
	private:
		using Block = ObjectPoolImpl::ObjectPoolBlock<T>;
		using IndexT = ObjectPoolImpl::IndexT;

		struct BlockInfo
		{
			IndexT freeBlockCount_ = 0;
			Block* blocks_ = nullptr;
			const T* memOffset_ = nullptr;
		};

		BlockInfo* AllocateBlock()
		{
			Block* newBlock = Block::Create(PerBlockSize);
			if (newBlock == nullptr) {
				return nullptr;
			}

			mBlockCount++;

			mBlockInfos = reinterpret_cast<BlockInfo*>(Memory::ReallocStatic(mBlockInfos, mBlockCount * sizeof(BlockInfo)));
			ERR_FAIL_COND(!mBlockInfos);

			BlockInfo& blockInfo = mBlockInfos[mBlockCount - 1];
			blockInfo.freeBlockCount_ = PerBlockSize;
			blockInfo.blocks_ = newBlock;
			blockInfo.memOffset_ = newBlock->GetMemOffset();

			return &blockInfo;
		}

		BlockInfo* mBlockInfos = nullptr;
		IndexT mBlockCount = 0;
		IndexT mFreeBlockIndex = 0;
		
		const IndexT PerBlockSize;

		ObjectPool(const ObjectPool& rhs) = delete;
		ObjectPool& operator=(const ObjectPool& rhs) = delete;

	public:
		ObjectPool(IndexT initalPerBlockSize) :
			PerBlockSize(initalPerBlockSize)
		{
			AllocateBlock();
		}

		~ObjectPool()
		{
			ObjectPoolStats stats = GetCurrentPoolStats();
			Debug::CheckAssertion(stats.numAllocations <= 0);

			BlockInfo* currentBlock = mBlockInfos;
			for (BlockInfo* blockEnd = mBlockInfos + mBlockCount; currentBlock != blockEnd; currentBlock++)
			{
				currentBlock->blocks_->DeleteAll();
				currentBlock->freeBlockCount_ = PerBlockSize;

				Block::Destroy(currentBlock->blocks_);
			}
			Memory::FreeStatic(mBlockInfos);
		}

		template<typename... Args>
		T* New(Args&&... args)
		{
			// 遍历所有block找到可用的block,在block中分配对应对象
			BlockInfo* currentBlock = mBlockInfos + mFreeBlockIndex;
			for (BlockInfo* blockEnd = mBlockInfos + mBlockCount; 
				currentBlock != blockEnd && currentBlock->freeBlockCount_ == 0; currentBlock++);

			mFreeBlockIndex = static_cast<IndexT>(currentBlock - mBlockInfos);

			if (mFreeBlockIndex == mBlockCount) 
			{
				currentBlock = AllocateBlock();
				if (currentBlock == nullptr) {
					Debug::Error("[ObjectPool] Failed to allocate memory.");
					return nullptr;
				}
			}

			T* ret = currentBlock->blocks_->New(std::forward<Args>(args)...);
			if (ret == nullptr) {
				Debug::Error("[ObjectPool] Failed to create new instance.");
				return nullptr;
			}
			currentBlock->freeBlockCount_--;

			return ret;
		}

		void Delete(const T* obj)
		{
			if (obj == nullptr) {
				return;
			}

			// 每个block记录了分配对象的启始和结束地址
			BlockInfo* currentBlock = mBlockInfos;
			for (BlockInfo* blockEnd = mBlockInfos + mBlockCount; currentBlock != blockEnd; currentBlock++)
			{
				const T* objectBegin = currentBlock->memOffset_;
				const T* objectEnd = objectBegin + PerBlockSize;
				if (obj >= objectBegin && obj < objectEnd)
				{
					currentBlock->blocks_->Delete(obj);
					currentBlock->freeBlockCount_++;

					IndexT freeBlockIndex = static_cast<IndexT>(currentBlock - mBlockInfos);
					if (freeBlockIndex < mFreeBlockIndex) {
						mFreeBlockIndex = freeBlockIndex;
					}
					break;
				}
			}
		}

		void DeleteAll()
		{
			BlockInfo* currentBlock = mBlockInfos;
			for (BlockInfo* blockEnd = mBlockInfos + mBlockCount; currentBlock != blockEnd; currentBlock++)
			{
				currentBlock->blocks_->DeleteAll();
				currentBlock->freeBlockCount_ = PerBlockSize;
			}
			mFreeBlockIndex = 0;
		}

		void ReclaimMemory()
		{
			// 将所有的空Block全部排列到后方
			IndexT usedIndex = mBlockCount;
			IndexT emptyIndex = mBlockCount;
			for (IndexT index = 0; index < mBlockCount; index++)
			{
				if (mBlockInfos[index].freeBlockCount_ < PerBlockSize)
				{
					usedIndex = index;
				}
				else if (index < emptyIndex)
				{
					emptyIndex = index;
				}

				if (emptyIndex < usedIndex && usedIndex != mBlockCount)
				{
					std::swap(mBlockInfos[usedIndex], mBlockInfos[emptyIndex]);
					usedIndex = emptyIndex;
					emptyIndex++;
				}
			}

			// all blocks is unused
			if (usedIndex == mBlockCount)
			{
				usedIndex = 0;
				mFreeBlockIndex = 0;
			}
		
			for (IndexT index = usedIndex + 1; index < mBlockCount; index++) {
				Block::Destroy(mBlockInfos[index].blocks_);
			}

			mBlockCount = usedIndex + 1;
			mBlockInfos = reinterpret_cast<BlockInfo*>(Memory::ReallocStatic(mBlockInfos, mBlockCount * sizeof(BlockInfo)));
			ERR_FAIL_COND(!mBlockInfos);

			mFreeBlockIndex = mBlockCount;
			for (IndexT index = 0; index < mBlockCount; index++)
			{
				if (mBlockInfos[index].freeBlockCount_ > 0)
				{
					mFreeBlockIndex = index;
					break;
				}
			}
		}

		ObjectPoolStats GetCurrentPoolStats()
		{
			ObjectPoolStats stats = {};
			stats.numBlocks = mBlockCount;
	
			BlockInfo* currentBlock = mBlockInfos;
			for (BlockInfo* blockEnd = mBlockInfos + mBlockCount; currentBlock != blockEnd; currentBlock++)
			{
				if (currentBlock->freeBlockCount_ < PerBlockSize) {
					stats.numBlocks += currentBlock->blocks_->GetAllocatedNum();
				}
			}
			return stats;
		}
	};
}
