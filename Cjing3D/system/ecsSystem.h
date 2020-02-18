#pragma once

#include "common\common.h"
#include "helper\IDGenerator.h"
#include "helper\archive.h"

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

	inline Entity DistrubuteEntity(Entity entity, U32 seed)
	{
		// 序列化后的entity可能会和已有entity重复，使用新种子^重新扰乱
		// entity,事先左移一位保证entity的符号位为正

		if (entity != INVALID_ENTITY && seed > 0) {
			entity = ((entity << 1) ^ seed) >> 1;
		}

		return entity;
	}

	inline Entity SerializeEntity(Archive& archive, U32 seed)
	{
		Entity entity = INVALID_ENTITY;
		archive >> entity;

		return DistrubuteEntity(entity, seed);
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

		inline void RemoveAndKeepSorted(Entity entity)
		{
			auto it = mLookup.find(entity);
			if (it != mLookup.end())
			{
				const auto index = it->second;
				const Entity entity = mEntities[index];

				// 如果删除的不是最后的元素，则将后面的元素往前移
				if (index < mComponents.size() - 1)
				{
					for (int i = index + 1; i < mComponents.size(); i++){
						mComponents[i - 1] = std::move(mComponents[i]);
					}

					for (int i = index + 1; i < mEntities.size(); i++) {
						mEntities[i - 1] = mEntities[i];
						mLookup[mEntities[i - 1]] = i - 1;
					}
				}

				mComponents.pop_back();
				mEntities.pop_back();
				mLookup.erase(entity);
			}
		}

		inline void MoveLastInto(U32 index)
		{
			// 将末尾的对象插入到指定位置
			
			if (index == GetCount() - 1 || GetCount() < 2) {
				return;
			}

			auto targetComponent = std::move(mComponents.back());
			auto targetEntity = mEntities.back();

			// 将拆入位置的元素往后移
			for (int i = mComponents.size() - 1; i > index; i--) {
				mComponents[i] = std::move(mComponents[i - 1]);
			}

			for (int i = mEntities.size() - 1; i > index; i--) {
				mEntities[i] = mEntities[i - 1];
				mLookup[mEntities[i]] = i;
			}

			mComponents[index] = std::move(targetComponent);
			mEntities[index] = targetEntity;
			mLookup[targetEntity] = index;
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

		inline ComponentTPtr GetOrCreateComponent(Entity entity)
		{
			auto ret = GetComponent(entity);
			if (ret != nullptr) {
				return ret;
			}
			return Create(entity);
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

		inline std::vector<Entity>& GetEntities()
		{
			return mEntities;
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

		void Serialize(Archive& archive, U32 seed = 0)
		{
			// 序列化时需要将原始数据清空
			Clear();

			// 数据结构为：entityCount components entities
			U32 count = 0;
			archive >> count;
			if (count <= 0) {
				return;
			}

			mComponents.resize(count);
			mEntities.resize(count);

			for (int i = 0; i < count; i++){
				mComponents[i] = std::make_shared<ComponentT>();
				mComponents[i]->Serialize(archive, seed);
			}
			for (int i = 0; i < count; i++) {
				Entity entity = SerializeEntity(archive, seed);
				mEntities[i] = entity;
				mLookup[entity] = i;
			}
		}

		void Unserialize(Archive& archive)const
		{
			archive << GetCount();
		
			for (auto& component : mComponents) {
				component->Unserialize(archive);
			}
			for (const auto& entity : mEntities) {
				archive << entity;
			}
		}

	private:
		std::vector<Entity> mEntities;
		std::vector<ComponentTPtr> mComponents;
		std::unordered_map<Entity, size_t> mLookup;
	};


}
}