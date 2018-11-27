#pragma once

#include "common\common.h"
#include "core\systemContext.hpp"
#include "world\actor.h"

namespace Cjing3D{

class World : public SubSystem
{
public:
	World(SystemContext& gameContext);
	virtual ~World();

	virtual void Initialize();
	virtual void Uninitialize();
	virtual void Update();

	/******************************************
	* Actor Method 
	*******************************************/
	ActorPtr CreateNewActor();
	void AddActor(ActorPtr actor);
	bool RemoveActor(ActorPtr actor);

	ActorPtr GetActorByName(const StringID& name);
	ActorPtr GetActorByGUID(U32 guid);
	std::vector<ActorPtr>& GetAllActor();
	const std::vector<ActorPtr>& GetAllActor()const;

	bool IsActorExists(ActorPtr actor);

private:

	std::vector<ActorPtr> mAllActors;
	std::map<U32, ActorPtr> mActorGuidMap;
	std::map<StringID, ActorPtr> mActorNameMap;
};

}