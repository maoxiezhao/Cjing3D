#include "engine.h"
#include "helper\fileSystem.h"
#include "helper\profiler.h"
#include "core\jobSystem.h"
#include "core\eventSystem.h"
#include "resource\resourceManager.h"
#include "renderer\renderer.h"
#include "world\world.h"

namespace Cjing3D
{

Engine::Engine(GameComponent* gameConponent):
	mIsInitialized(false),
	mWindowHwnd(nullptr),
	mWindowHinstance(nullptr),
	mRenderingDeviceType(RenderingDeviceType_D3D11)
{
	mSystemContext = std::make_unique<SystemContext>();

	mGameComponent = std::unique_ptr<GameComponent>(gameConponent);
	mGameComponent->SetGameContext(mSystemContext.get());
}

Engine::~Engine()
{
}

void Engine::Initialize()
{
	if (mIsInitialized == true) {
		return;
	}

#ifdef CJING_DEBUG
	Debug::SetDebugConsoleEnable(true);
	Debug::InitializeDebugConsole();
#endif

	// initialize engine time
	mTimer.Start();
	mTime = mTimer.GetTime();
	mSystemContext->SetEngineTime(mTime);

	// initialize file data
	std::string dataPath = "./../Assets";
	if (!FileData::OpenData("", dataPath))
		Debug::Die("No data file was found int the direcion:" + dataPath);
	
	// initialize job system
	auto jobSystem = new JobSystem(*mSystemContext, false);
	mSystemContext->RegisterSubSystem(jobSystem);
	jobSystem->Initialize();
	
	// setup gamecomponent 
	mGameComponent->Setup();

	// initialize resource manager
	auto resourceManager = new ResourceManager(*mSystemContext);
	mSystemContext->RegisterSubSystem(resourceManager);
	resourceManager->Initialize();

	// initialize renderer
	auto renderer = new Renderer(*mSystemContext, mRenderingDeviceType, (HWND)mWindowHwnd);
	mSystemContext->RegisterSubSystem(renderer);
	renderer->Initialize();

	// initialize world
	auto world = new World(*mSystemContext);
	mSystemContext->RegisterSubSystem(world);
	world->Initialize();

	mGameComponent->Initialize();

	Profiler::GetInstance().BeginFrame();
	mIsInitialized = true;
}

void Engine::Tick()
{
	// begin frame
	auto& profiler = Profiler::GetInstance();
	profiler.BeginFrame();

	mTime = mTimer.GetTime();
	mSystemContext->SetEngineTime(mTime);

	auto& renderer = mSystemContext->GetSubSystem<Renderer>();
	auto& world = mSystemContext->GetSubSystem<World>();

	profiler.BeginBlock("Update");
	FIRE_EVENT(EventType::EVENT_TICK);
	world.Update();
	renderer.Update();
	profiler.EndBlock();

	profiler.BeginBlock("Render");
	FIRE_EVENT(EventType::EVENT_RENDER);
	renderer.Render();
	profiler.EndBlock();

	profiler.BeginBlock("Compose");
	renderer.Compose();
	renderer.Present();
	profiler.EndBlock();

	// end frame
	profiler.EndFrame();
}

void Engine::Uninitialize()
{
	if (mIsInitialized == false) {
		return;
	}

	Profiler::GetInstance().EndFrame();

	mGameComponent->Uninitialize();

	mSystemContext->GetSubSystem<World>().Uninitialize();
	mSystemContext->GetSubSystem<Renderer>().Uninitialize();
	mSystemContext->GetSubSystem<ResourceManager>().Uninitialize();
	mSystemContext->GetSubSystem<JobSystem>().Uninitialize();

	FileData::CloseData();

	mTimer.Stop();

	mIsInitialized = false;
}

void Engine::SetHandles(void * windowHwnd, void * windowInstance)
{
	mWindowHwnd = windowHwnd;
	mWindowHinstance = windowInstance;
}

}