#include "engine.h"
#include "helper\fileSystem.h"
#include "helper\profiler.h"
#include "helper\enumInfo.h"
#include "core\jobSystem.h"
#include "core\eventSystem.h"
#include "input\InputSystem.h"
#include "resource\resourceManager.h"
#include "renderer\renderer.h"
#include "scripts\luaContext.h"
#include "gui\guiStage.h"
#include "system\sceneSystem.h"

namespace Cjing3D
{
namespace {
	Renderer* mRenderer = nullptr;
	LuaContext* mLuaContext = nullptr;
	GUIStage* mGuiStage = nullptr;
	InputManager* mInputManager = nullptr;
}

Engine::Engine(GameComponent* gameConponent):
	mIsInitialized(false),
	mWindowHwnd(nullptr),
	mWindowHinstance(nullptr),
	mRenderingDeviceType(RenderingDeviceType_D3D11)
{
	mSystemContext = &SystemContext::GetSystemContext();

	mGameComponent = std::unique_ptr<GameComponent>(gameConponent);
	mGameComponent->SetGameContext(mSystemContext);
}

Engine::~Engine()
{
	Uninitialize();
}

void Engine::Initialize()
{
	if (mIsInitialized == true) {
		return;
	}

	Memory::Initialize();

	Profiler::GetInstance().Initialize();
	Profiler::GetInstance().SetProfileEnable(true);

#ifdef CJING_DEBUG
	Debug::SetDebugConsoleEnable(true);
	Debug::SetDieOnError(true);
	Debug::InitializeDebugConsole();
#endif
	Logger::PrintConsoleHeader();
	Logger::Info("Initialize engine modules");

	// initialize engine time
	mTimer.Start();
	mEngineTime = mTimer.GetTime();
	mDeltaTimeAccumulator = 0;

	mSystemContext->SetEngineTime(mEngineTime);
	mSystemContext->SetEngine(this);

	// initialize file data
	std::string dataPath = ".\\..\\Assets";
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

	// initialize gui stage
	auto guiStage = new GUIStage(*mSystemContext);
	mSystemContext->RegisterSubSystem(guiStage);
	guiStage->Initialize();

	// initialize lua context
	auto luaContext = new LuaContext(*mSystemContext);
	mSystemContext->RegisterSubSystem(luaContext);
	luaContext->Initialize();

	// initialize main scene
	Scene::Initialize();

	// game start
	mGameComponent->Initialize();
	luaContext->OnMainStart();

	mRenderer = renderer;
	mLuaContext = luaContext;
	mGuiStage = guiStage;
	mInputManager = inputSystem;

	mIsInitialized = true;
}

void Engine::Tick()
{
	PROFILER_BEGIN_FRAME();
	PROFILER_OPTICK_EVENT();

	mEngineTime = mTimer.GetTime();
	mSystemContext->SetEngineTime(mEngineTime);
	F32 deltaTime = F32(mEngineTime.deltaTime / 1000.0f);

	bool isWindowActive = mBaseWindow->IsWindowActive();
	isWindowActive = true;
	if (isWindowActive)
	{
		const F32 dt = mIsLockFrameRate ? (1.0f / mTargetFrameRate) : deltaTime;
		if (!mIsSkipFrame) 
		{
			FixedUpdate();
		}
		else
		{
			mDeltaTimeAccumulator += dt;

			// 某些情况会触发超时操作，此时需要重置计数
			if (mDeltaTimeAccumulator > 8) {
				mDeltaTimeAccumulator = 0;
			}

			const F32 targetRateInv = 1.0f / mTargetFrameRate;
			while (mDeltaTimeAccumulator >= targetRateInv)
			{
				FixedUpdate();
				mDeltaTimeAccumulator -= targetRateInv;
			}
		}

		Update(dt);
		UpdateInput(dt);
		Render();
	}
	else
	{
		mDeltaTimeAccumulator = 0;
	}

	EndFrame();

	// end frame
	PROFILER_END_FRAME();
}

void Engine::Uninitialize()
{
	if (mIsInitialized == false) {
		return;
	}

	mSystemContext->GetSubSystem<LuaContext>().OnMainUninitialize();
	mGameComponent->Uninitialize();

	Scene::GetScene().Clear();
	Scene::Uninitialize();

	mSystemContext->GetSubSystem<GUIStage>().Uninitialize();
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
	Profiler::GetInstance().Uninitialize();

	Memory::Uninitialize();

	mIsInitialized = false;
}

void Engine::SetHandles(void * windowHwnd, void * windowInstance)
{
	mWindowHwnd = windowHwnd;
	mWindowHinstance = windowInstance;
}

void Engine::FixedUpdate()
{
	PROFILER_BEGIN_GPU_BLOCK("FixedUpdate");
	FIRE_EVENT(EventType::EVENT_FIXED_TICK);

	mGameComponent->FixedUpdate();
	mLuaContext->FixedUpdate();
	mRenderer->FixedUpdate();

	PROFILER_END_BLOCK();
}

void Engine::Update(F32 deltaTime)
{
	PROFILER_BEGIN_GPU_BLOCK("Update");
	FIRE_EVENT(EventType::EVENT_TICK);

	mLuaContext->Update(deltaTime);
	mGameComponent->Update(mEngineTime);
	mGuiStage->Update(deltaTime);
	mRenderer->Update(deltaTime);

	PROFILER_END_BLOCK();
}

void Engine::UpdateInput(F32 deltaTime)
{
	PROFILER_BEGIN_GPU_BLOCK("Input");
	mInputManager->Update(deltaTime);
	PROFILER_END_BLOCK();
}

void Engine::Render()
{
	PROFILER_BEGIN_GPU_BLOCK("Render");
	FIRE_EVENT(EventType::EVENT_RENDER);
	mRenderer->Render();
	PROFILER_END_BLOCK();
}

void Engine::EndFrame()
{
	mRenderer->Present();
	mLuaContext->GC();
}

}