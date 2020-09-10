#pragma once

#include "common\common.h"

#include <mutex>
#include <thread>
#include <memory>
#include <map>

namespace Cjing3D {

	/**
	*	\brief ��������Դ��
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
				auto resource = it->second.lock();	// lock�������������Դ��shared_ptr
				if (resource)
					return resource;

				// �ͷ������õ���Դ
				mResourceMap.erase(it);
			}

			// ����������򴴽��µ���Դ
			auto newResource = std::make_shared<AutoResource>(
				*this, key, std::forward<Args>(args)...);
			mResourceMap.emplace(key, newResource);

			return newResource;
		}

		// �Ƴ�ָ��Key��Resoure 
		void Remove(const KeyT& key)
		{
			const std::lock_guard<std::mutex> lock(mMutex);
			auto it = mResourceMap.find(key);
			if (it != mResourceMap.end())
				mResourceMap.erase(it);
		}

		// �ͷ�ResourcePool
		void RemoveAll()
		{
			const std::lock_guard<std::mutex> lock(mMutex);
			mResourceMap.clear();
		}

		// �ж��Ƿ��
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
				auto resource = it->second.lock();	// lock�������������Դ��shared_ptr
				if (resource)
					return resource;

				// �ͷ������õ���Դ
				mResourceMap.erase(it);
			}
			return nullptr;
		}

	private:
		// ����ʵ��δ������Դ�Զ��ͷ�
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
				// ������ʱ�ͷ���Դ��
				mResourcePool.Remove(mKey);
			}
		private:
			ResourcePool & mResourcePool;
			const KeyT mKey;
		};

		std::map<KeyT, std::weak_ptr<ResourceT> > mResourceMap;	/** �������Դ�������õ���ʽ���棬�����������ʱ�ͷ� */
		mutable std::mutex mMutex;

	};

	/**
	*	\brief �־û���Դ��
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


			// ����������򴴽��µ���Դ
			auto newResource = std::make_shared<ResourceT>(
				std::forward<Args>(args)...);
			mResourceMap.emplace(key, newResource);

			return newResource;
		}

		// �Ƴ�ָ��Key��Resoure 
		void Remove(const KeyT& key)
		{
			const std::lock_guard<std::mutex> lock(mMutex);
			auto it = mResourceMap.find(key);
			if (it != mResourceMap.end())
				mResourceMap.erase(it);
		}

		// �ͷ�ResourcePool
		void RemoveAll()
		{
			const std::lock_guard<std::mutex> lock(mMutex);
			mResourceMap.clear();
		}

		// �ж��Ƿ��
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
		std::map<KeyT, std::shared_ptr<ResourceT> > mResourceMap;	/** �������Դ�������õ���ʽ���棬�����������ʱ�ͷ� */
		mutable std::mutex mMutex;

	};
}