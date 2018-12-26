#include "world.h"

namespace Cjing3D {

World::World(SystemContext& gameContext) :
	SubSystem(gameContext)
{
}


World::~World()
{
}

void World::Initialize()
{
}

void World::Uninitialize()
{
}

void World::Update()
{
}

void World::Clear()
{
	mAllMaterial.clear();
	mAllActors.clear();
}

ActorPtr World::CreateNewActor()
{
	return ActorPtr();
}

void World::AddActor(ActorPtr actor)
{
}

bool World::RemoveActor(ActorPtr actor)
{
	return false;
}

ActorPtr World::GetActorByName(const StringID & name)
{
	return ActorPtr();
}

ActorPtr World::GetActorByGUID(U32 guid)
{
	return ActorPtr();
}

std::vector<ActorPtr>& World::GetAllActor()
{
	return mAllActors;
}

const std::vector<ActorPtr>& World::GetAllActor() const
{
	return mAllActors;
}

bool World::IsActorExists(ActorPtr actor)
{
	return false;
}

MaterialPtr World::GetMaterialByGUID(U32 guid)
{
	auto findIt = mAllMaterial.find(guid);
	if (findIt != mAllMaterial.end())
	{
		return findIt->second;
	}
	return nullptr;
}

}