#pragma once

#include "common\common.h"
#include "helper\IDGenerator.h"
#include "helper\archive.h"
#include "utils\objectPool.h"

#include <unordered_map>
#include <vector>

namespace Cjing3D
{

// Entity component system
namespace ECS
{
	using Entity = U32;
	static const Entity INVALID_ENTITY = 0;
	static const uint32_t COMPONENT_POOL_DEFAULT_BLOCK_SIZE = 32;

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
		//using ComponentTPtr = std::shared_ptr<ComponentT>;
		using ComponentTPtr = ComponentT*;

		ComponentManager(size_t capacity = 0)
		{
			mEntities.reserve(capacity);
			mComponents.reserve(capacity);
			mLookup.reserve(capacity);
		}

		~ComponentManager()
		{
			Clear();
		}

		ComponentManager(ComponentManager& rhs) = delete;
		ComponentManager& operator=(const ComponentManager& rhs) = delete;

		static void Initialize()
		{
			mComponentPool = CJING_MEM_NEW(ObjectPool<ComponentT>(COMPONENT_POOL_DEFAULT_BLOCK_SIZE));
		}

		static void Uninitilize()
		{
			CJING_MEM_DELETE(mComponentPool);
		}

		static ComponentT* CreateComponent()
		{
			// TODO:optim
			if (mComponentPool == nullptr) {
				return CJING_MEM_NEW(ComponentT);
			} else {
				return mComponentPool->New();
			}
		}

		static void DestroyComponent(ComponentT* ptr)
		{
			// TODO:optim
			if (mComponentPool == nullptr) {
				return CJING_MEM_DELETE(ptr);
			}
			else {
				return mComponentPool->Delete(ptr);
			}
		}

		inline void Clear(bool clearPool = true)
		{
			if (clearPool) {
				for (auto& componentPtr : mComponents) {
					DestroyComponent(componentPtr);
				}
			}

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

			// 必须要保证component和entity的index一致
			mLookup[entity] = mComponents.size();
			mEntities.push_back(entity);

			// 从object pool创建
			auto componentPtr = CreateComponent();
			mComponents.push_back(componentPtr);

			// TEMP: 临时尝试下在component中保存entity
			componentPtr->SetCurrentEntity(entity);

			return componentPtr;
		}

		inline void Remove(Entity entity)
		{
			auto it = mLookup.find(entity);
			if (it != mLookup.end())
			{
				const auto index = it->second;
				const Entity entity = mEntities[index];

				ComponentTPtr ptr = nullptr;
				// 如果删除的不是最后的元素，则与最后的元素交换
				if (index < mComponents.size() - 1)
				{
					ptr = mComponents[index];
					mComponents[index] = std::move(mComponents.back());
					mEntities[index] = mEntities.back();
					mLookup[mEntities.back()] = index;
				}

				if (ptr != nullptr) {
					DestroyComponent(ptr);
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

				ComponentTPtr ptr = mComponents[index];
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

				if (ptr != nullptr) {
					DestroyComponent(ptr);
				}

				mEntities.pop_back();
				mLookup.erase(entity);
			}
		}

		inline void MoveInto(U32 from, U32 into)
		{
			if (from >= GetCount() || into >= GetCount() || from == into) {
				return;
			}

			auto targetComponent = std::move(mComponents[from]);
			auto targetEntity = mEntities[from];

			int dir = from < into ? 1 : -1;
			for (int i = from; i != into; i+= dir) {
				int nextIndex = i + dir;
				mComponents[i] = std::move(mComponents[nextIndex]);
				mEntities[i] = mEntities[nextIndex];
				mLookup[mEntities[i]] = i;
			}

			mComponents[into] = std::move(targetComponent);
			mEntities[into] = targetEntity;
			mLookup[targetEntity] = into;
		}

		inline void MoveLastInto(U32 index)
		{
			// 将末尾的对象插入到指定位置
			MoveInto(GetCount() - 1, index);
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

					// TODO: 可能还会产生大量拷贝构造行为
					int tSize = sizeof(ComponentT);
					auto componentPtr = other.GetComponent(entity);
					mComponents.push_back(componentPtr);
				}
			}

			// merge的时候，other的component不需要释放
			other.Clear(false);
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

		inline const ComponentTPtr GetComponentByIndex(U32 index)const
		{
			Debug::CheckAssertion(index >= 0 && index < mComponents.size());
			return mComponents[index];
		}

		inline std::vector<ComponentTPtr>& GetComponents()
		{
			return mComponents;
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

			// 从object pool创建
			for (int i = 0; i < count; i++){
				mComponents[i] = CreateComponent();
				mComponents[i]->Serialize(archive, seed);
			}
			for (int i = 0; i < count; i++) {
				Entity entity = SerializeEntity(archive, seed);
				mEntities[i] = entity;
				mLookup[entity] = i;

				// TEMP: 临时尝试下在component中保存entity
				mComponents[i]->SetCurrentEntity(entity);
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

		static ObjectPool<ComponentT>* mComponentPool;
	};

	template<typename ComponentT>
	ObjectPool<ComponentT>* ComponentManager<ComponentT>::mComponentPool = nullptr;
}
}