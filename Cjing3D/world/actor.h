#pragma once

#include "common\common.h"
#include "core\systemContext.hpp"
#include "world\component\component.h"
#include "world\component\renderable.h"
#include "world\component\transform.h"

#include <map>

namespace Cjing3D{

class Actor : public std::enable_shared_from_this<Actor>
{
public:
	Actor(SystemContext& gameContext);
	virtual ~Actor();

	virtual void Initialize();
	virtual void Uninitialize();
	virtual void Update();
	virtual void Clone();

	/******************************************
	* Component Method 
	*******************************************/
	template< typename ComponentT>
	std::shared_ptr<ComponentT> AddComponent();
	void AddComponent(ComponentPtr component);
	ComponentPtr AddComponent(Component_Type type);

	template< typename ComponentT>
	std::vector<std::shared_ptr<ComponentT>> GetComponents();
	std::vector<ComponentPtr> GetComponents(Component_Type type);

	bool HasComponent(Component& component);
	bool HasComponent(Component_Type type);
	template<typename T>
	bool HasComponent();

	void RemoveComponent(U32 guid);
	void RemoveComponent(Component_Type type);
	template< typename T>
	void RemoveComponent();

	template <typename ActionT>
	void ForEachComponent(ActionT&& action);

	/******************************************
	* Common Method
	*******************************************/
	StringID GetName()const { return mName; }
	void SetName(const StringID& name) { mName = name; }
	U32 GetGUID()const { return mGuid; }
	bool HasParent()const { return !mParent.expired(); }
	std::shared_ptr<Actor> GetParent() { return mParent.lock(); }

	void AddChild(std::shared_ptr<Actor> actor);
	void RemoveChild(std::shared_ptr<Actor> actor);
	void RemoveAllChild();

	template< typename ActionT>
	void ForEachChild(ActionT&& action);

private:
	SystemContext& mGameContext;

	std::unique_ptr<Transform> mTransform;

	U32 mGuid;
	StringID mName;
	std::weak_ptr<Actor> mParent;
	std::vector<std::shared_ptr<Actor> > mChilds;

	// component sets
	std::map<U32, ComponentPtr> mComponentGUIDMap;
	std::vector<ComponentPtr> mComponents;
};

using ActorPtr = std::shared_ptr<Actor>;
using ActorPtrArray = std::vector<ActorPtr>;

template< typename ComponentT>
std::shared_ptr<ComponentT> Actor::AddComponent()
{
	Component_Type type = Component::DeduceComponentType<ComponentT>();
	if (type == ComponentType_Unknown) {
		return nullptr;
	}

	auto component = std::make_shared<ComponentT>();
	component->SetType(type);
	component->Initialize();

	mComponentGUIDMap.insert(std::make_pair(component->GetGUID(), component));
	mComponents.push_back(component);

	return component;
}

template<typename ComponentT>
inline std::vector<std::shared_ptr<ComponentT>> Actor::GetComponents()
{
	std::vector<std::shared_ptr<ComponentT>> components;
	Component_Type type = Component::DeduceComponentType<ComponentT>();
	if (type == ComponentType_Unknown) {
		return components;
	}

	for (auto& component : mComponents)
	{
		if (component->GetType() == type) {
			components.push_back(std::dynamic_pointer_cast<ComponentT>(component));
		}
	}
	return components;
}

template<typename T>
inline bool Actor::HasComponent()
{
	Component_Type type = Component::DeduceComponentType<T>();
	if (type == ComponentType_Unknown) {
		return false;
	}
	
	return HasComponent(type);
}

template<typename T>
inline void Actor::RemoveComponent()
{
	Component_Type type = Component::DeduceComponentType<T>();
	if (type == ComponentType_Unknown) {
		return;
	}

	for (auto it = mComponents.begin(); it != mComponents.end();)
	{
		auto& component = *it;
		if (component->GetType() == type) {
			component->Uninitialize();
			component.reset();

			mComponentGUIDMap.erase(component->GetGUID());
			it = mComponents.erase(it);
		}
		else {
			it++;
		}
	}
}

template<typename ActionT>
inline void Actor::ForEachComponent(ActionT && action)
{
	for (auto& component : mComponents) {
		action(component);
	}
}

template<typename ActionT>
inline void Actor::ForEachChild(ActionT && action)
{
	for (auto& actor : mChilds) {
		action(actor);
	}
}

}