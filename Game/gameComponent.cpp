#include "gameComponent.h"
#include "resource\modelImporter.h"

namespace Cjing3D
{
	TestGame::TestGame()
	{
	}

	TestGame::~TestGame()
	{
	}

	void TestGame::Setup()
	{
		auto systemContext = GetGameContext();
		ImportModelObj("..\\Assets\\Models\\cornell_box.obj", *systemContext);
	}

	void TestGame::Initialize()
	{
	}

	void TestGame::Update(EngineTime time)
	{
	}

	void TestGame::Uninitialize()
	{
	}
}