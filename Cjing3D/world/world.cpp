#include "world.h"

namespace Cjing3D {

World::World(GameContext& gameContext) :
	GameSystem(gameContext)
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
	// TODO: 在此处插入 return 语句
}

const std::vector<ActorPtr>& World::GetAllActor() const
{
	// TODO: 在此处插入 return 语句
}

bool World::IsActorExists(ActorPtr actor)
{
	return false;
}

}