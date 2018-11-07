#pragma once

#include "common\common.h"
#include "core\gameContext.hpp"
#include "world\world.h"

namespace Cjing3D{

class World : public GameSystem
{
public:
	World(GameContext& gameContext);
	virtual ~World();

	virtual void Initialize();
	virtual void Uninitialize();
	virtual void Update();

	/******************************************
	* Actor Method 
	*******************************************/
	ActorPtr GetActorByName(const StringID& name);
	ActorPtr GetActorByGUID(U32 guid);
	std::vector<ActorPtr>& GetAllActor();

private:

	std::vector<ActorPtr> mAllActors;
	std::map<U32, ActorPtr> mActorGuidMap;
	std::map<StringID, ActorPtr> mActorNameMap;
};

}