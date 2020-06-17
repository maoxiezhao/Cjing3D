#pragma once

#include "common\common.h"

#include <mutex>
#include <thread>
#include <memory>
#include <map>

namespace Cjing3D {

	/**
	*	\brief 弱引用资源池
	*/
	template<typename KeyT, typename ResourceT>
	class ResourcePool
	{
	public:
		class AutoResource;
		using KeyType = KeyT;
		using PoolResourceType = ResourceT;

		ResourcePool() = default;
		~ResourcePool() {
			RemoveAll();
		}

		template< typename... Args>
		std::shared_ptr<ResourceT> GetOrCreate(const KeyT& key, Args&&... args)
		{
			const std::lock_guard<std::mutex> lock(mMutex);

			auto it = mResourceMap.find(key);
			if (it != mResourceMap.end())
			{
				auto resource = it->second.lock();	// lock创建被管理的资源的shared_ptr
				if (resource)
					return resource;

				// 释放无引用的资源
				mResourceMap.erase(it);
			}

			// 如果不存在则创建新的资源
			auto newResource = std::make_shared<AutoResource>(
				*this, key, std::forward<Args>(args)...);
			mResourceMap.emplace(key, newResource);

			return newResource;
		}

		// 移除指定Key的Resoure 
		void Remove(const KeyT& key)
		{
			const std::lock_guard<std::mutex> lock(mMutex);
			auto it = mResourceMap.find(key);
			if (it != mResourceMap.end())
				mResourceMap.erase(it);
		}

		// 释放ResourcePool
		void RemoveAll()
		{
			const std::lock_guard<std::mutex> lock(mMutex);
			mResourceMap.clear();
		}

		// 判断是否空
		bool Empty()const
		{
			const std::lock_guard<std::mutex> lock(mMutex);
			return mResourceMap.empty();
		}

		bool Contains(const KeyType& key)const
		{
			const std::lock_guard<std::mutex> lock(mMutex);

			auto it = mResourceMap.find(key);
			if (it != mResourceMap.end()){
				return true;
			}
			return false;
		}

		std::shared_ptr<ResourceT> Get(const KeyType& key)
		{
			const std::lock_guard<std::mutex> lock(mMutex);

			auto it = mResourceMap.find(key);
			if (it != mResourceMap.end())
			{
				auto resource = it->second.lock();	// lock创建被管理的资源的shared_ptr
				if (resource)
					return resource;

				// 释放无引用的资源
				mResourceMap.erase(it);
			}
			return nullptr;
		}

	private:
		// 用于实现未引用资源自动释放
		class AutoResource : public PoolResourceType
		{
		public:
			template<typename... Args>
			AutoResource(ResourcePool& resourcePool,
				const KeyT& key,
				Args&&... args) :
				PoolResourceType(std::forward<Args>(args)...),
				mResourcePool(resourcePool),
				mKey(key) {}

			virtual~AutoResource() {
				// 当销毁时释放资源池
				mResourcePool.Remove(mKey);
			}
		private:
			ResourcePool & mResourcePool;
			const KeyT mKey;
		};

		std::map<KeyT, std::weak_ptr<ResourceT> > mResourceMap;	/** 对象池资源以弱引用的形式保存，当外界无引用时释放 */
		mutable std::mutex mMutex;

	};

	/**
	*	\brief 持久化资源池
	*/
	template<typename KeyT, typename ResourceT>
	class PersistentResourcePool
	{
	public:
		using KeyType = KeyT;
		using PoolResourceType = ResourceT;

		PersistentResourcePool() = default;
		~PersistentResourcePool() {
			RemoveAll();
		}

		template< typename... Args>
		std::shared_ptr<ResourceT> GetOrCreate(const KeyT& key, Args&&... args)
		{
			const std::lock_guard<std::mutex> lock(mMutex);

			auto it = mResourceMap.find(key);
			if (it != mResourceMap.end())
				return it->second;


			// 如果不存在则创建新的资源
			auto newResource = std::make_shared<ResourceT>(
				std::forward<Args>(args)...);
			mResourceMap.emplace(key, newResource);

			return newResource;
		}

		// 移除指定Key的Resoure 
		void Remove(const KeyT& key)
		{
			const std::lock_guard<std::mutex> lock(mMutex);
			auto it = mResourceMap.find(key);
			if (it != mResourceMap.end())
				mResourceMap.erase(it);
		}

		// 释放ResourcePool
		void RemoveAll()
		{
			const std::lock_guard<std::mutex> lock(mMutex);
			mResourceMap.clear();
		}

		// 判断是否空
		bool Empty()const
		{
			const std::lock_guard<std::mutex> lock(mMutex);
			return mResourceMap.empty();
		}

		bool Contains(const KeyType& key)const
		{
			const std::lock_guard<std::mutex> lock(mMutex);

			auto it = mResourceMap.find(key);
			return (it != mResourceMap.end());
		}

		std::shared_ptr<ResourceT> Get(const KeyType& key)
		{
			const std::lock_guard<std::mutex> lock(mMutex);

			auto it = mResourceMap.find(key);
			return (it != mResourceMap.end()) ? it->second : nullptr;
		}

	private:
		std::map<KeyT, std::shared_ptr<ResourceT> > mResourceMap;	/** 对象池资源以弱引用的形式保存，当外界无引用时释放 */
		mutable std::mutex mMutex;

	};
}