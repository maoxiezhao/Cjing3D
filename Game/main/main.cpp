#include "engine.h"
#include "platform\win32\gameWindowWin32.h"
#include "game\gameEditor.h"

#include <functional>

using namespace Cjing3D;

std::unique_ptr<GameWindowWin32> mainWindow = nullptr;
std::unique_ptr<Engine> mainEngine = nullptr;

int WINAPI WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nShowCmd)
{
	// initialize
	mainWindow = std::make_unique<GameWindowWin32>(
		"Cjing3D dev v0.0.1",
		I32x2(1440, 800),
		false);
	mainWindow->Show();

	mainEngine = std::make_unique<Engine>(new GameEditor());
	mainEngine->SetHandles(mainWindow->GetHwnd(), mainWindow->GetInstance());
	mainEngine->SetWindow(mainWindow.get());
	mainEngine->Initialize();

	// run
	mainWindow->RunWindow(*mainEngine);

	// uninitialize
	mainEngine->Uninitialize();
	mainWindow->ShutDown();

	return 0;
}