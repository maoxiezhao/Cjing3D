#include "engine.h"
#include "helper\fileSystem.h"
#include "helper\profiler.h"
#include "helper\enumInfo.h"
#include "core\jobSystem.h"
#include "core\eventSystem.h"
#include "core\settings.h"
#include "input\InputSystem.h"
#include "resource\resourceManager.h"
#include "renderer\renderer.h"
#include "scripts\luaContext.h"
#include "gui\guiStage.h"
#include "system\sceneSystem.h"
#include "audio\audio.h"
#include "audio\audioDeviceX.h"

namespace Cjing3D
{
namespace {
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
	mSystemContext = &GlobalContext::GetGlobalContext();

	mGameComponent = std::unique_ptr<GameComponent>(gameConponent);
	mGameComponent->SetGameContext(mSystemContext);
}

Engine::~Engine()
{
	Uninitialize();
}

void Engine::Initialize(const std::string& assetPath, const std::string& assetName)
{
	if (mIsInitialized == true) {
		return;
	}

	// initialize memory system
	Memory::Initialize();

	// initialize profiler
	Profiler::GetInstance().Initialize();

#ifdef CJING_DEBUG
	Profiler::GetInstance().SetProfileEnable(true);
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
	if (!FileData::OpenData("", assetPath, assetName)) 
	{
		Debug::Die("No data file was found int the direcion:" + assetPath);
		return;
	}
	
	// initialize setting
	mSystemContext->RegisterSubSystem<Settings>();
	// setup gamecomponent 
	mGameComponent->Setup();

	// initialize sub systems
	mSystemContext->RegisterSubSystem<JobSystem>();
	mSystemContext->RegisterSubSystem<InputManager>((HWND)mWindowHwnd, (HINSTANCE)mWindowHinstance);
	mSystemContext->RegisterSubSystem<Audio::AudioManager>(CJING_MEM_NEW(Audio::AudioDeviceX));
	mSystemContext->RegisterSubSystem<ResourceManager>();
	mSystemContext->RegisterSubSystem<RendererSystem>(mRenderingDeviceType, (HWND)mWindowHwnd);
	mSystemContext->RegisterSubSystem<GUIStage>();
	mSystemContext->RegisterSubSystem<LuaContext>();

	mLuaContext   = &mSystemContext->GetSubSystem<LuaContext>();
	mGuiStage     = &mSystemContext->GetSubSystem<GUIStage>();
	mInputManager = &mSystemContext->GetSubSystem<InputManager>();

	// initialize main scene
	Scene::Initialize();

	// game context initialize
	mGameComponent->Initialize();

	// do main scirpt ::OnStart()
	mLuaContext->StartMainScript();

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

	// stop timer
	mTimer.Stop();

	// stop script and game
	mSystemContext->GetSubSystem<LuaContext>().StopMainScript();
	mGameComponent->Uninitialize();

	// clear scene
	Scene::GetScene().Clear();
	Scene::Uninitialize();

	// clear profilers
	Profiler::GetInstance().Clear();

	// clear sub systems
	mSystemContext->Uninitialize();

	FileData::CloseData();

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
	PROFILER_BEGIN_CPU_BLOCK("FixedUpdate");
	FIRE_EVENT(EventType::EVENT_FIXED_TICK);

	mGameComponent->FixedUpdate();
	mLuaContext->FixedUpdate();
	mGuiStage->FixedUpdate();
	Renderer::FixedUpdate();

	PROFILER_END_BLOCK();
}

void Engine::Update(F32 deltaTime)
{
	PROFILER_BEGIN_CPU_BLOCK("Update");
	FIRE_EVENT(EventType::EVENT_TICK);

	mLuaContext->Update(deltaTime);
	mGameComponent->Update(mEngineTime);
	mGuiStage->Update(deltaTime);	
	Renderer::Update(deltaTime);

	PROFILER_END_BLOCK();
}

void Engine::UpdateInput(F32 deltaTime)
{
	PROFILER_BEGIN_CPU_BLOCK("Input");
	mInputManager->Update(deltaTime);
	PROFILER_END_BLOCK();
}

void Engine::Render()
{
	PROFILER_BEGIN_CPU_BLOCK("PreRender");
	mGameComponent->PreRender();
	PROFILER_END_BLOCK();

	PROFILER_BEGIN_CPU_BLOCK("Render");
	FIRE_EVENT(EventType::EVENT_RENDER);
	Renderer::Render();
	PROFILER_END_BLOCK();
}

void Engine::EndFrame()
{
	Renderer::Present();
	mLuaContext->GC();
}

}