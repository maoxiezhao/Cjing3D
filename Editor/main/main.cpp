
#include "editor\gameEditor.h"
#include "platform\win32\gameAppWin32.h"

int WINAPI WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nShowCmd)
{
	Cjing3D::Win32::GameAppWin32 gameApp;
	gameApp.SetInstance(hInstance);
	gameApp.SetAssetPath(".", "Assets");
	gameApp.SetScreenSize({ DEFAULT_GAME_WINDOW_WIDTH, DEFAULT_GAME_WINDOW_HEIGHT });
	gameApp.SetTitleName(std::string("Cjing3D ") + CjingVersion::GetVersionString());

	gameApp.Run([](const std::shared_ptr<Engine>& engine) {
		return std::make_unique<GameEditor>(engine);
	});

	return 0;
}