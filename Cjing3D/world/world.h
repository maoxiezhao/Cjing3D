#pragma once

#include "common\common.h"
#include "core\systemContext.hpp"
#include "core\sceneSystem.h"
#include "world\actor.h"
#include "renderer\components\material.h"
#include "renderer\components\mesh.h"

namespace Cjing3D{

/**
*	\brief 世界类管理所有的Actor对象，同时还包括Material资源和Mesh资源
*/
class World : public SubSystem
{
public:
	World(SystemContext& gameContext);
	virtual ~World();

	virtual void Initialize();
	virtual void Uninitialize();
	virtual void Update();
	virtual void Clear();

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

	MaterialPtr GetMaterialByGUID(U32 guid);

	Scene& GetMainScene();

private:

	std::vector<ActorPtr> mAllActors;
	std::map<U32, ActorPtr> mActorGuidMap;
	std::map<StringID, ActorPtr> mActorNameMap;

	std::map<U32, MaterialPtr> mAllMaterial;	// key is guid

	Scene mMainScene;
};

}