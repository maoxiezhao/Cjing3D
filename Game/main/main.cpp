
#include "game\levelEditor\levelEditor.h"
#include "platform\win32\gameAppWin32.h"

int WINAPI WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nShowCmd)
{
	return Cjing3D::GameAppWin32("GameAssets").Run(new CjingGame::LevelEditor());
}