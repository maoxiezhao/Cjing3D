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
	mMainScene.Update();
}

void World::Clear()
{
	mMainScene.Clear();
}

Scene & World::GetMainScene()
{
	return mMainScene;
}

}