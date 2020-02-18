#include "engine.h"
#include "window.h"
#include "gameComponent.h"

#include <functional>

using namespace Cjing3D;

std::unique_ptr<Window> mainWindow = nullptr;
std::unique_ptr<Engine> mainEngine = nullptr;

class ApplicationMessageHandler : public WindowMessageHandler
{
public:
	ApplicationMessageHandler() {}
	virtual~ApplicationMessageHandler() {}

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
std::shared_ptr<ApplicationMessageHandler> mAppHandler = nullptr;

int Run();

int WINAPI WinMain(HINSTANCE instance,
	HINSTANCE preInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	// initialize
	mainWindow = std::make_unique<Window>(
		"Cjing3D dev v0.0.1",
		I32x2(1440, 800),
		false);
	mAppHandler = std::make_shared<ApplicationMessageHandler>();
	mainWindow->AddMessageHandler(mAppHandler);
	mainWindow->Show();

	mainEngine = std::make_unique<Engine>(new TestGame());
	mainEngine->SetHandles(mainWindow->GetHwnd(), mainWindow->GetInstance());
	mainEngine->SetWindow(mainWindow.get());
	mainEngine->Initialize();

	Run();

	// uninitialize
	mainEngine->Uninitialize();
	mainEngine.reset();
	mainWindow.reset();

	return 0;
}

int Run()
{
	MSG msg;
	SecureZeroMemory(&msg, sizeof(msg));
	while (msg.message != WM_QUIT && !mainEngine->GetIsExiting())
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			continue;
		}
	
		mainEngine->Tick();
	}
	return static_cast<int> (msg.wParam);
}