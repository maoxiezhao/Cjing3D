#include "engine.h"
#include "core\gameComponent.h"
#include "helper\fileSystem.h"

namespace Cjing3D
{

namespace {
	/**
	*	\brief 自定义的消息处理
	*/
	class EngineMessageHandler : public WindowMessageHandler
	{
	public:
		EngineMessageHandler() {}
		virtual~EngineMessageHandler() {}

		std::function<void(bool actived)> OnActiveChange;

		virtual bool HandleMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT& result)
		{
			switch (message)
			{
			case WM_ACTIVATEAPP:
				bool deactive = static_cast<bool>(!wParam);
				if (OnActiveChange)
				{
					OnActiveChange(deactive);
					result = 0;
					return true;
				}
				break;
			}

			return false;
		}
	};
	std::shared_ptr<EngineMessageHandler> mAppHandler = nullptr;
}

Engine::Engine():
	mMainWindow(),
	mIsInitialized(false)
{
	Initialize();
}

Engine::~Engine()
{
	Uninitialize();
}

int Engine::Run(std::shared_ptr<GameComponent> gameComponent)
{
	if (gameComponent == nullptr) {
		Debug::Error("The game component is null");
		return 0;
	}

	// init game component
	mGameComponent = gameComponent;
	gameComponent->Initialize(*this);

	// show window
	mMainWindow->Show();

	// message loop
	MSG msg;
	SecureZeroMemory(&msg, sizeof(msg));
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			continue;
		}

		gameComponent->Run(mTimer);
	}

	return static_cast<int> (msg.wParam);
}

Window & Engine::GetMainWindow()
{
	return *mMainWindow;
}

void Engine::Initialize()
{
	if (mIsInitialized == true) {
		return;
	}

	Debug::SetDebugConsoleEnable(true);
	Debug::InitializeDebugConsole();

	std::string dataPath = "../Assets";
	if (!FileData::OpenData("", dataPath))
		Debug::Die("No data file was found int the direcion:" + dataPath);

	mTimer.Start();

	mMainWindow = std::make_unique<Window>(
		"Cjing3D",
		I32x2(1024, 768),
		false);
	mAppHandler = std::make_shared<EngineMessageHandler>();
	mMainWindow->AddMessageHandler(mAppHandler);

	mIsInitialized = true;
}

void Engine::Uninitialize()
{
	if (mIsInitialized == false) {
		return;
	}

	if (mGameComponent != nullptr){
		mGameComponent->Uninitialize();
	}

	mMainWindow.reset();
	mTimer.Stop();

	FileData::CloseData();

	mIsInitialized = false;
}

}