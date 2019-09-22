#include "engine.h"
#include "helper\fileSystem.h"
#include "helper\profiler.h"
#include "core\jobSystem.h"
#include "core\eventSystem.h"
#include "input\InputSystem.h"
#include "resource\resourceManager.h"
#include "renderer\renderer.h"
#include "scripts\luaContext.h"

// TEST
#include "helper\enumInfo.h"

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
	Logger::PrintConsoleHeader();
	Logger::Info("Initialize engine modules");

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

	// initialize input system
	auto inputSystem = new InputManager(*mSystemContext);
	mSystemContext->RegisterSubSystem(inputSystem);
	inputSystem->Initialize((HWND)mWindowHwnd, (HINSTANCE)mWindowHinstance);

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

	// initialize lua context
	auto luaContext = new LuaContext(*mSystemContext);
	mSystemContext->RegisterSubSystem(luaContext);
	luaContext->Initialize();

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

	F32 deltaTime = F32(mTime.deltaTime.count());
	auto& inputManager = mSystemContext->GetSubSystem<InputManager>();
	inputManager.Update(deltaTime);

#ifdef CJING_DEBUG
	if (inputManager.IsKeyDown(KeyCode::Esc)) {
		SetIsExiting(true);
	}
#endif

	auto& renderer = mSystemContext->GetSubSystem<Renderer>();
	auto& luaContext = mSystemContext->GetSubSystem<LuaContext>();

	profiler.BeginBlock("Update");
	FIRE_EVENT(EventType::EVENT_TICK);
	mGameComponent->Update(mTime);
	renderer.Update(deltaTime);
	luaContext.Update(deltaTime);
	profiler.EndBlock();

	profiler.BeginBlock("Render");
	FIRE_EVENT(EventType::EVENT_RENDER);
	renderer.Render();
	profiler.EndBlock();

	profiler.BeginBlock("Compose");
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

	mSystemContext->GetSubSystem<LuaContext>().Uninitialize();
	mSystemContext->GetSubSystem<Renderer>().Uninitialize();
	mSystemContext->GetSubSystem<ResourceManager>().Uninitialize();
	mSystemContext->GetSubSystem<InputManager>().Uninitialize();
	mSystemContext->GetSubSystem<JobSystem>().Uninitialize();

	FileData::CloseData();

	mTimer.Stop();

#ifdef CJING_DEBUG
	Debug::UninitializeDebugConsolse();
#endif

	mIsInitialized = false;
}

void Engine::SetHandles(void * windowHwnd, void * windowInstance)
{
	mWindowHwnd = windowHwnd;
	mWindowHinstance = windowInstance;
}

}