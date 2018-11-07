#pragma once

#include "common\common.h"
#include "core\gameContext.hpp"
#include "world\component\component.h"

#include <map>

namespace Cjing3D{

class Actor : public std::enable_shared_from_this<Actor>
{
public:
	Actor(GameContext& gameContext);
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
	ComponentPtr AddComponent(ComponentType type);

	template< typename ComponentT>
	std::vector<std::shared_ptr<ComponentT>> GetComponents();
	std::vector<ComponentPtr> GetComponents(ComponentType type);

	bool HasComponent(ComponentType type);
	template<typename T>
	bool HasComponent();

	void RemoveComponent(U32 guid);
	void RemoveComponent(ComponentType type);
	template< typename T>
	void RemoveComponent();

	StringID GetName()const { return mName; }
	void SetName(const StringID& name) { mName = name; }

	U32 GetGUID()const { return mGuid; }

private:
	GameContext& mGameContext;

	U32 mGuid;
	StringID mName;

	// component sets
	std::map<U32, ComponentPtr> mComponentGUIDMap;
	std::multimap<ComponentType, ComponentPtr> mComponents;
};

using ActorPtr = std::shared_ptr<Actor>;

}