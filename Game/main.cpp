#include <windows.h>

#include "test.h"

using namespace Cjing3D;

int WINAPI WinMain(HINSTANCE instance,
	HINSTANCE preInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	Engine engine;
	engine.Run(std::make_shared<TestGameComponent>());

	return 0;
}