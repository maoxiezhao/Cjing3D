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

	}

	void TestGame::Initialize()
	{
		auto systemContext = GetGameContext();
		ModelImporter::ImportModelObj("..\\Assets\\Models\\cornell_box.obj", *systemContext);
	}

	void TestGame::Update(EngineTime time)
	{
	}

	void TestGame::Uninitialize()
	{
	}
}