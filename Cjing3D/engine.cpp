#include "engine.h"
#include "helper\fileSystem.h"
#include "core\jobSystem.h"
#include "resource\resouceManager.h"

namespace Cjing3D
{

Engine::Engine():
	mIsInitialized(false),
	mWindowHwnd(nullptr),
	mWindowHinstance(nullptr)
{
	Initialize();
}

Engine::~Engine()
{
	Uninitialize();
}

//int Engine::Run(std::shared_ptr<GameComponent> gameComponent)
//{
//	if (gameComponent == nullptr) {
//		Debug::Error("The game component is null");
//		return 0;
//	}
//
//	// init game component
//	//mGameComponent = gameComponent;
//	//gameComponent->Initialize(*this);
//
//	//// show window
//	//mMainWindow->Show();
//
//	//// message loop
//	//MSG msg;
//	//SecureZeroMemory(&msg, sizeof(msg));
//	//while (msg.message != WM_QUIT)
//	//{
//	//	if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
//	//		TranslateMessage(&msg);
//	//		DispatchMessage(&msg);
//	//		continue;
//	//	}
//
//	//	gameComponent->Run(mTimer);
//	//}
//
//	//return static_cast<int> (msg.wParam);
//
//	return 0;
//}

void Engine::Initialize()
{
	if (mIsInitialized == true) {
		return;
	}

#ifdef CJING_DEBUG
	Debug::SetDebugConsoleEnable(true);
	Debug::InitializeDebugConsole();
#endif

	// initialize file data
	std::string dataPath = "./../Assets";
	if (!FileData::OpenData("", dataPath))
		Debug::Die("No data file was found int the direcion:" + dataPath);

	mGameContext = std::make_unique<SystemContext>();

	// initialize engine time
	mTimer.Start();
	mGameContext->SetEngineTime(mTime);
	
	auto jobSystem = new jobSystem(*mGameContext)
	jobSystem->Initialize();
	mGameContext->RegisterSubSystem(jobSystem);

	auto resourceManager = new ResourceManager(*mGameContext);
	resourceManager->Initialize();
	mGameContext->RegisterSubSystem(resourceManager);

	mIsInitialized = true;
}

void Engine::Update()
{
	mGameContext->SetEngineTime(mTime);
}

void Engine::Uninitialize()
{
	if (mIsInitialized == false) {
		return;
	}

	//if (mGameComponent != nullptr){
	//	mGameComponent->Uninitialize();
	//}

	//mMainWindow.reset();

	mTimer.Stop();

	FileData::CloseData();

	mIsInitialized = false;
}

void Engine::SetHandles(void * windowHwnd, void * windowInstance)
{
	mWindowHwnd = windowHwnd;
	mWindowHinstance = windowInstance;
}

}