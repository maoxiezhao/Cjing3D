#pragma once

#include "common\common.h"
#include "helper\IDGenerator.h"

#include <unordered_map>
#include <vector>

namespace Cjing3D
{

// Entity component system
namespace ECS
{
	using Entity = U32;
	static const Entity INVALID_ENTITY = 0;
	inline Entity CreateEntity()
	{
		return GENERATE_RANDOM_ID;
	}

	// Component 管理器，提供entity对Component映射
	template<typename ComponentT>
	class ComponentManager
	{
	public:
		using ComponentTPtr = std::shared_ptr<ComponentT>;

		ComponentManager(size_t capacity = 0)
		{
			mEntities.reserve(capacity);
			mComponents.reserve(capacity);
			mLookup.reserve(capacity);
		}

		inline void Clear()
		{
			mEntities.clear();
			mComponents.clear();
			mLookup.clear();
		}
		
		inline ComponentTPtr Create(Entity entity)
		{
			Debug::CheckAssertion(entity != INVALID_ENTITY, "Invalid entity.");
			Debug::CheckAssertion(mLookup.find(entity) == mLookup.end(), "The entity is already have component");
			Debug::CheckAssertion(mEntities.size() == mComponents.size());
			Debug::CheckAssertion(mLookup.size() == mComponents.size());

			mLookup[entity] = mComponents.size();
			mEntities.push_back(entity);

			auto componentPtr = std::make_shared<ComponentT>();
			mComponents.push_back(componentPtr);

			return componentPtr;
		}

		inline void Remove(Entity entity)
		{
			auto it = mLookup.find(entity);
			if (it != mLookup.end())
			{
				const auto index = it->second;
				const Entity entity = mEntities[index];

				// 如果删除的不是最后的元素，则与最后的元素交换
				if (index < mComponents.size() - 1)
				{
					mComponents[index] = std::move(mComponents.back());
					mEntities[index] = mEntities.back();
					mLookup[mEntities.back()] = index;
				}

				mComponents.pop_back();
				mEntities.pop_back();
				mLookup.erase(entity);
			}
		}

		inline void Merge(ComponentManager<ComponentT>& other)
		{
			const auto newSize = GetCount() + other.GetCount();
			mComponents.reserve(newSize);
			mEntities.reserve(newSize);
			mLookup.reserve(newSize);

			for (size_t i = 0; i < other.GetCount(); i++)
			{
				Entity entity = other.GetEntityByIndex(i);
				if (Contains(entity) == false)
				{
					mEntities.push_back(entity);
					mLookup[entity] = mComponents.size();

					auto componentPtr = other.GetComponent(entity);
					mComponents.push_back(componentPtr);
				}
			}

			other.Clear();
		}

		inline bool Contains(Entity entity)
		{
			return mLookup.find(entity) != mLookup.end();
		}

		inline ComponentTPtr GetComponent(Entity entity)
		{
			auto it = mLookup.find(entity);
			if (it != mLookup.end())
			{
				return mComponents[it->second];
			}
			return nullptr;
		}

		inline const ComponentTPtr GetComponent(Entity entity)const 
		{
			auto it = mLookup.find(entity);
			if (it != mLookup.end())
			{
				return mComponents[it->second];
			}
			return nullptr;
		}

		inline size_t GetEntityIndex(Entity entity)
		{
			auto it = mLookup.find(entity);
			if (it != mLookup.end())
			{
				return it->second;
			}
			return ~0;
		}

		inline size_t GetCount() const 
		{
			return mComponents.size();
		}

		inline Entity GetEntityByIndex(size_t index)
		{
			if (index >= 0 && index < mEntities.size())
			{
				return mEntities[index];
			}
			return INVALID_ENTITY;
		}

		template< typename ActionT>
		inline void ForEachComponent(ActionT&& action)
		{
			for (size_t i = 0; i < mComponents.size(); i++)
			{
				auto entity = mEntities[i];
				auto component = mComponents[i];
				action(entity, component);
			}
		}

		inline ComponentTPtr operator[](size_t index)
		{
			return mComponents[index];
		}

		inline const ComponentTPtr operator[](size_t index) const
		{
			return mComponents[index];
		}

	private:
		std::vector<Entity> mEntities;
		std::vector<ComponentTPtr> mComponents;
		std::unordered_map<Entity, size_t> mLookup;
	};


}
}